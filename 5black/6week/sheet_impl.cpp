#include "sheet_impl.h"

#include <stdexcept>

#include "cell_impl.h"
#include "exception.h"

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
  throw runtime_error("unimplemented");
}

void Sheet::InsertCols(int before, int count) {
  throw runtime_error("unimplemented");
}

void Sheet::DeleteRows(int first, int count) {
  throw runtime_error("unimplemented");
}

void Sheet::DeleteCols(int first, int count) {
  throw runtime_error("unimplemented");
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
    cell_ref = make_unique<Cell>(*this);
  }

  return *cell_ref.get();
}
