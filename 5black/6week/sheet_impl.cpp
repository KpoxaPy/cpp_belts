#include "sheet_impl.h"

#include <stdexcept>
#include <ostream>

#include "cell_impl.h"

using namespace std;

Sheet::Sheet() {
  table_.reserve(Position::kMaxCols);
}

const Cell* Sheet::GetSpecificCell(Position pos) const {
  if (!pos.IsValid()) {
    throw InvalidPositionException("GetCell: invalid position");
  }

  if (static_cast<size_t>(pos.col) < table_.size() && static_cast<size_t>(pos.row) < table_[pos.col].size()) {
    return table_[pos.col][pos.row].get();
  }

  return nullptr;
}

Cell* Sheet::GetSpecificCell(Position pos) {
  if (!pos.IsValid()) {
    throw InvalidPositionException("GetCell: invalid position");
  }

  if (static_cast<size_t>(pos.col) < table_.size() && static_cast<size_t>(pos.row) < table_[pos.col].size()) {
    return table_[pos.col][pos.row].get();
  }

  return nullptr;
}

Cell& Sheet::GetReferencedCell(Position pos) {
  if (!pos.IsValid()) {
    throw InvalidPositionException("GetCell: invalid position");
  }

  AddCellToPrintable(pos);
  return InsertCell(pos);
}

void Sheet::SetCell(Position pos, std::string text) {
  if (!pos.IsValid()) {
    throw InvalidPositionException("SetCell: invalid position");
  }

  AddCellToPrintable(pos);
  InsertCell(pos).SetText(text);
}

const ICell* Sheet::GetCell(Position pos) const {
  return GetSpecificCell(pos);
}

ICell* Sheet::GetCell(Position pos) {
  return GetSpecificCell(pos);
}

void Sheet::ClearCell(Position pos) {
  if (!pos.IsValid()) {
    throw InvalidPositionException("ClearCell: invalid position");
  }

  if (static_cast<size_t>(pos.col) < table_.size() && static_cast<size_t>(pos.row) < table_[pos.col].size()) {
    auto& cell_ptr = table_[pos.col][pos.row];
    if (cell_ptr) {
      cell_ptr->SetText("");
      if (cell_ptr->GetReferencingCells().empty()) {
        cell_ptr.reset();
      }
    }
  }
}

void Sheet::InsertRows(int before, int count) {
  if (size_.rows + count > Position::kMaxRows) {
    throw TableTooBigException("");
  }

  for (int col = 0; col < size_.cols; ++col) {
    if (before < static_cast<int>(table_[col].size())) {
      table_[col].insert(table_[col].begin() + before, count, {});
    }
  }

  if (before < size_.rows) {
    size_.rows += count;
  } else {
    size_.rows = before + count;
  }

  for (auto& col : table_) {
    for (auto& cell_ptr : col) {
      if (cell_ptr && cell_ptr.get()) {
        Cell& cell = *cell_ptr.get();
        if (auto formula = cell.GetFormula(); formula) {
          formula->HandleInsertedRows(before, count);
        }
      }
    }
  }
}

void Sheet::InsertCols(int before, int count) {
  if (size_.cols + count > Position::kMaxCols) {
    throw TableTooBigException("");
  }

  if (before < size_.cols) {
    size_.cols += count;
    table_.insert(table_.begin() + before, count, {});
    for (int col = before; col < before + count; ++col) {
      table_[col].reserve(Position::kMaxRows);
    }
  } else {
    size_t old_size = size_.cols;
    size_.cols = before + count;
    table_.resize(size_.cols);
    for (int col = old_size; col < size_.cols; ++col) {
      table_[col].reserve(Position::kMaxRows);
    } 
  }

  for (auto& col : table_) {
    for (auto& cell_ptr : col) {
      if (cell_ptr && cell_ptr.get()) {
        Cell& cell = *cell_ptr.get();
        if (auto formula = cell.GetFormula(); formula) {
          formula->HandleInsertedCols(before, count);
        }
      }
    }
  }
}

void Sheet::DeleteRows(int first, int count) {
  if (first >= size_.rows) {
    return;
  }

  for (int col = 0; col < size_.cols; ++col) {
    if (first < static_cast<int>(table_[col].size())) {
      const int actual_count = std::min(count, static_cast<int>(table_[col].size()) - first);
      for (int row = first; row < first + actual_count; ++row) {
        auto& cell_ptr = table_[col][row];
        if (cell_ptr && cell_ptr.get()) {
          cell_ptr.get()->PropagadeRefsTo(false);
        }
      }
      table_[col].erase(table_[col].begin() + first, table_[col].begin() + first + actual_count);
    }
  }

  size_.rows -= std::min(count, size_.rows - first);

  for (auto& col : table_) {
    for (auto& cell_ptr : col) {
      if (cell_ptr && cell_ptr.get()) {
        Cell& cell = *cell_ptr.get();
        if (auto formula = cell.GetFormula(); formula) {
          formula->HandleDeletedRows(first, count);
        }
      }
    }
  }
}

void Sheet::DeleteCols(int first, int count) {
  if (first >= size_.cols) {
    return;
  }

  const int actual_count = std::min(count, size_.cols - first);

  for (int col = first; col < first + actual_count; ++col) {
    for (size_t row = 0; row < table_[col].size(); ++row) {
      auto& cell_ptr = table_[col][row];
      if (cell_ptr && cell_ptr.get()) {
        cell_ptr.get()->PropagadeRefsTo(false);
      }
    }
  }
  table_.erase(table_.begin() + first, table_.begin() + first + actual_count);

  size_.cols -= actual_count;

  for (auto& col : table_) {
    for (auto& cell_ptr : col) {
      if (cell_ptr && cell_ptr.get()) {
        Cell& cell = *cell_ptr.get();
        if (auto formula = cell.GetFormula(); formula) {
          formula->HandleDeletedCols(first, count);
        }
      }
    }
  }
}

Size Sheet::GetPrintableSize() const {
  return size_;
}

void Sheet::PrintValues(std::ostream& output) const {
  for (int row = 0; row < size_.rows; ++row) {
    for (int col = 0; col < size_.cols; ++col) {
      if (col > 0) {
        output << '\t';
      }
      auto cell_ptr = GetCell({row, col});
      if (cell_ptr) {
        visit([&output](auto&& arg) {
            output << arg;
          }, cell_ptr->GetValue());
      }
    }
    output << '\n';
  }
}

void Sheet::PrintTexts(std::ostream& output) const {
  for (int row = 0; row < size_.rows; ++row) {
    for (int col = 0; col < size_.cols; ++col) {
      if (col > 0) {
        output << '\t';
      }
      auto cell_ptr = GetCell({row, col});
      if (cell_ptr) {
        output << cell_ptr->GetText();
      }
    }
    output << '\n';
  }
}

void Sheet::AddCellToPrintable(Position pos) {
  if (pos.col >= size_.cols) {
    size_.cols = pos.col + 1;
  }
  if (pos.row >= size_.rows) {
    size_.rows = pos.row + 1;
  }
}

Cell& Sheet::InsertCell(Position pos) {
  if (static_cast<size_t>(pos.col) >= table_.size()) {
    size_t i = table_.size();
    table_.resize(pos.col + 1);
    for (; i < table_.size(); ++i) {
      table_[i].reserve(Position::kMaxRows);
    }
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
