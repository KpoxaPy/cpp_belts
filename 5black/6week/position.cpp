#include "position.h"

#include <stdexcept>
#include <tuple>

using namespace std;

bool Position::operator==(const Position& rhs) const {
  return tie(col, row) == tie(rhs.col, rhs.row);
}

bool Position::IsValid() const {
  if (col < 0 || kMaxCols < col) {
    return false;
  }

  if (row < 0 || kMaxRows < row) {
    return false;
  }

  return true;
}

std::string Position::ToString() const {
  throw runtime_error("unimplemented");
}

Position Position::FromString(std::string_view str) {
  throw runtime_error("unimplemented");
}
