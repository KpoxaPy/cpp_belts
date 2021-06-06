#include "position.h"

#include <list>
#include <sstream>
#include <tuple>

#include "exception.h"
#include "hashing.h"

using namespace std;

bool Position::operator==(const Position& rhs) const {
  return tie(col, row) == tie(rhs.col, rhs.row);
}

bool Position::IsValid() const {
  if (col < 0 || kMaxCols <= col) {
    return false;
  }

  if (row < 0 || kMaxRows <= row) {
    return false;
  }

  return true;
}

std::string Position::ToString() const {
  ostringstream ss;
  
  if (IsValid()) {
    list<char> c;
    int cc = col;

    while (cc >= 0) {
      c.push_front('A' + cc % 26);
      cc = cc / 26 - 1;
    }

    ss << string(begin(c), end(c)) << row + 1;
  }

  return ss.str();
}

Position Position::FromString(std::string_view str) {
  std::string_view ss = str;
  Position res = {0, -1};
  while (!ss.empty() && 'A' <= ss[0] && ss[0] <= 'Z') {
    res.col = ss[0] - 'A' + (res.col + 1) * 26;
    ss.remove_prefix(1);
  }
  while (!ss.empty() && '0' <= ss[0] && ss[0] <= '9') {
    res.row = ss[0] - '0' + res.row * 10;
    ss.remove_prefix(1);
  }

  if (!ss.empty()) {
    return {-1, -1};
  }

  res.row -= 1;
  return res;
}

std::size_t std::hash<Position>::operator()(const Position& pos) const noexcept {
  size_t seed = 0;
  HashCombine(seed, int_hasher(pos.col));
  HashCombine(seed, int_hasher(pos.row));
  return seed;
}