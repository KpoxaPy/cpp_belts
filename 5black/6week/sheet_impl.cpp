#include "sheet_impl.h"

#include <stdexcept>
#include <ostream>
#include <sstream>

#include "profile.h"
#include "cell_impl.h"

using namespace std;

std::string Sheet::GetStats() const {
  ostringstream ss;
  ss << "Set: " << m_set.Get() << "\n";
  ss << "Value: " << m_value.Get() << "\n";
  ss << "Insert: " << m_insert.Get() << "\n";
  ss << "Clear: " << m_clear.Get() << "\n";
  ss << "InsertRow: " << m_row_insert.Get() << "\n";
  ss << "InsertCol: " << m_col_insert.Get() << "\n";
  ss << "DeleteRow: " << m_row_delete.Get() << "\n";
  ss << "DeleteCol: " << m_col_delete.Get() << "\n";

  ss << "Cell::Set: " << m_cell_set.Get() << "\n";
  ss << "Cell::Refs: " << m_cell_refs.Get() << "\n";
  ss << "Cell::CheckCircular: " << m_cell_check_circular.Get() << "\n";
  ss << "Cell::InvalidateCache: " << m_cell_invalidate.Get() << "\n";
  ss << "Cell::FormulaParsing: " << m_cell_set_formula_parsing.Get() << "\n";
  return ss.str();
}

void Sheet::SetCell(Position pos, std::string text) {
  METER_DURATION(m_set);
  if (!pos.IsValid()) {
    throw InvalidPositionException("SetCell: invalid position");
  }

  InsertCell(pos).SetText(text);
}

const ICell* Sheet::GetCell(Position pos) const {
  if (!pos.IsValid()) {
    throw InvalidPositionException("GetCell: invalid position");
  }

  if (pos.col < static_cast<int>(table_.size()) && pos.row < static_cast<int>(table_[pos.col].size())) {
    return table_[pos.col][pos.row].get();
  }
  return nullptr;
}

ICell* Sheet::GetCell(Position pos) {
  if (!pos.IsValid()) {
    throw InvalidPositionException("GetCell: invalid position");
  }

  if (pos.col < static_cast<int>(table_.size()) && pos.row < static_cast<int>(table_[pos.col].size())) {
    return table_[pos.col][pos.row].get();
  }
  return nullptr;
}

Cell& Sheet::InsertCell(Position pos) {
  METER_DURATION(m_insert);
  if (static_cast<size_t>(pos.col) >= table_.size()) {
    table_.resize(pos.col + 1);
  }
  if (static_cast<size_t>(pos.row) >= table_[pos.col].size()) {
    table_[pos.col].resize(pos.row + 1);
  }

  auto& cell_ref = table_[pos.col][pos.row];
  if (!cell_ref) {
    cell_ref = make_shared<Cell>(*this);
  }
  return *cell_ref.get();
}

void Sheet::ClearCell(Position pos) {
  METER_DURATION(m_clear);
  if (!pos.IsValid()) {
    throw InvalidPositionException("ClearCell: invalid position");
  }

  if (!(pos.col < static_cast<int>(table_.size())) || !(pos.row < static_cast<int>(table_[pos.col].size()))) {
    return;
  }

  bool need_fit = false;
  if (auto& cell_ptr = table_[pos.col][pos.row]; cell_ptr) {
    cell_ptr->SetText("");
    if (cell_ptr->IsFree()) {
      cell_ptr.reset();
      need_fit = true;
    }
  }

  // Если у нас была удалена логическая ячейка, то
  // приводим реальный размер таблицы с соответствием с логическим
  // при этом ёмкость векторов остаётся строго в диапазоне максимальных позиций
  if (need_fit) {
    auto s = GetSize();
    if (pos.col >= s.cols) {
      table_.resize(s.cols);
    } else if (pos.row >= s.rows) {
      table_[pos.col].resize(s.rows);
    }
  }
}

void Sheet::InsertRows(int before, int count) {
  METER_DURATION(m_row_insert);
  const auto s = GetSize();
  if (s.rows + count > Position::kMaxRows) {
    throw TableTooBigException("");
  }

  // Вставка после области с ячейками ничего не меняет
  if (before >= s.rows) {
    return;
  }

  for (int col = 0; col < s.cols; ++col) {
    if (before < static_cast<int>(table_[col].size())) {
      table_[col].insert(table_[col].begin() + before, count, {});
    }
  }

  ProcessNonEmptyCells([before, count](Position, Cell& cell) {
    cell.HandleInsertedRows(before, count);
  });
}

void Sheet::InsertCols(int before, int count) {
  METER_DURATION(m_col_insert);
  const auto s = GetSize();
  if (s.cols + count > Position::kMaxCols) {
    throw TableTooBigException("");
  }

  // Вставка после области с ячейками ничего не меняет
  if (before >= s.cols) {
    return;
  }
  table_.insert(table_.begin() + before, count, {});

  ProcessNonEmptyCells([before, count](Position, Cell& cell) {
    cell.HandleInsertedCols(before, count);
  });
}

void Sheet::DeleteRows(int first, int count) {
  METER_DURATION(m_row_delete);
  const auto s = GetSize();

  // Удаление после области с ячейками ничего не меняет
  if (first >= s.rows) {
    return;
  }

  for (int col = 0; col < s.cols; ++col) {
    if (first < static_cast<int>(table_[col].size())) {
      const int actual_count = std::min(count, static_cast<int>(table_[col].size()) - first);
      for (int row = first; row < first + actual_count; ++row) {
        if (auto& cell_ptr = table_[col][row]; cell_ptr) {
          cell_ptr->PrepareToDelete();
        }
      }
      table_[col].erase(table_[col].begin() + first, table_[col].begin() + first + actual_count);
    }
  }

  ProcessNonEmptyCells([first, count](Position, Cell& cell) {
    cell.HandleDeletedRows(first, count);
  });
}

void Sheet::DeleteCols(int first, int count) {
  METER_DURATION(m_col_delete);
  const auto s = GetSize();

  // Удаление после конца области с ячейками ничего не меняет
  if (first >= s.cols) {
    return;
  }

  const int actual_count = std::min(count, s.cols - first);
  for (int col = first; col < first + actual_count; ++col) {
    for (size_t row = 0; row < table_[col].size(); ++row) {
      if (auto& cell_ptr = table_[col][row]; cell_ptr) {
        cell_ptr->PrepareToDelete();
      }
    }
  }
  table_.erase(table_.begin() + first, table_.begin() + first + actual_count);

  ProcessNonEmptyCells([first, count](Position, Cell& cell) {
    cell.HandleDeletedCols(first, count);
  });
}

Size Sheet::GetPrintableSize() const {
  Size s = {0, 0};
  ProcessNonEmptyCells([&s](Position p, Cell& cell) {
    if (!cell.GetText().empty()) {
      if (p.col + 1 > s.cols) {
        s.cols = p.col + 1;
      }
      if (p.row + 1 > s.rows) {
        s.rows = p.row + 1;
      }
    }
  });
  return s;
}

void Sheet::PrintValues(std::ostream& output) const {
  auto s = GetPrintableSize();
  for (int row = 0; row < s.rows; ++row) {
    for (int col = 0; col < s.cols; ++col) {
      if (col > 0) {
        output << '\t';
      }
      if (auto cell_ptr = GetCell({row, col}); cell_ptr) {
        visit([&output](auto&& arg) {
            output << arg;
          }, cell_ptr->GetValue());
      }
    }
    output << '\n';
  }
}

void Sheet::PrintTexts(std::ostream& output) const {
  auto s = GetPrintableSize();
  for (int row = 0; row < s.rows; ++row) {
    for (int col = 0; col < s.cols; ++col) {
      if (col > 0) {
        output << '\t';
      }
      if (auto cell_ptr = GetCell({row, col}); cell_ptr) {
        output << cell_ptr->GetText();
      }
    }
    output << '\n';
  }
}

void Sheet::ProcessNonEmptyCells(std::function<void(Position, Cell&)> f) const {
  for (int col = 0; col < static_cast<int>(table_.size()); ++col) {
    for (int row = 0; row < static_cast<int>(table_[col].size()); ++row) {
      if (auto ptr = table_[col][row].get(); ptr) {
        f({row, col}, *ptr);
      }
    }
  }
}

Size Sheet::GetSize() const {
  Size s = {0, 0};
  ProcessNonEmptyCells([&s](Position p, Cell& cell) {
    if (p.col + 1 > s.cols) {
      s.cols = p.col + 1;
    }
    if (p.row + 1 > s.rows) {
      s.rows = p.row + 1;
    }
  });
  return s;
}