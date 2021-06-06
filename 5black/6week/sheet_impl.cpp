#include "sheet_impl.h"

#include <stdexcept>

#include "cell_impl.h"
#include "exception.h"

using namespace std;

Sheet::Sheet() {
}

Sheet::~Sheet() {
}

void Sheet::SetCell(Position pos, std::string text) {
  if (!pos.IsValid()) {
    throw InvalidPositionException("SetCell: invalid position");
  }

  auto& cell_ptr = cells_[pos];
  // adjust printable

  if (!cell_ptr) {
    cell_ptr = make_unique<Cell>();
  }

  cell_ptr->SetText(std::move(text));
}

const ICell* Sheet::GetCell(Position pos) const {
  if (!pos.IsValid()) {
    throw InvalidPositionException("const GetCell: invalid position");
  }

  if (auto it = cells_.find(pos); it != cells_.end()) {
    return it->second.get();
  }
  return nullptr;
}

ICell* Sheet::GetCell(Position pos) {
  if (!pos.IsValid()) {
    throw InvalidPositionException("GetCell: invalid position");
  }

  if (auto it = cells_.find(pos); it != cells_.end()) {
    return it->second.get();
  }
  return nullptr;
}

void Sheet::ClearCell(Position pos) {
  if (!pos.IsValid()) {
    throw InvalidPositionException("ClearCell: invalid position");
  }

  cells_.erase(pos);
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
  return printable_size_;
}

void Sheet::PrintValues(std::ostream& output) const {
  throw runtime_error("unimplemented");
}

void Sheet::PrintTexts(std::ostream& output) const {
  throw runtime_error("unimplemented");
}