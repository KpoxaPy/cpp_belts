#include "sheet_impl.h"

#include <stdexcept>

#include "exception.h"

using namespace std;

void Sheet::SetCell(Position pos, std::string text) {
  if (!pos.IsValid()) {
    throw InvalidPositionException("SetCell: invalid position");
  }
  throw runtime_error("unimplemented");
}

const ICell* Sheet::GetCell(Position pos) const {
  if (!pos.IsValid()) {
    throw InvalidPositionException("GetCell: invalid position");
  }
  throw runtime_error("unimplemented");
}

ICell* Sheet::GetCell(Position pos) {
  if (!pos.IsValid()) {
    throw InvalidPositionException("GetCell: invalid position");
  }
  throw runtime_error("unimplemented");
}

void Sheet::ClearCell(Position pos) {
  if (!pos.IsValid()) {
    throw InvalidPositionException("ClearCell: invalid position");
  }
  throw runtime_error("unimplemented");
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
  throw runtime_error("unimplemented");
}

void Sheet::PrintValues(std::ostream& output) const {
  throw runtime_error("unimplemented");
}

void Sheet::PrintTexts(std::ostream& output) const {
  throw runtime_error("unimplemented");
}