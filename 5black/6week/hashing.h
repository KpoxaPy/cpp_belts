#pragma once

#include <utility>

inline void HashCombine(std::size_t& seed, std::size_t other) {
  seed ^= other + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}