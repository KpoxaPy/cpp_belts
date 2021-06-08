#pragma once

#include "common.h"
#include "formula.h"

namespace std {

template <>
struct hash<Position> {
  std::size_t operator()(const Position& pos) const noexcept;

  hash<int> int_hasher;
};

} // namespace std