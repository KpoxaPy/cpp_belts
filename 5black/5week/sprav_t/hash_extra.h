#pragma once

#include <functional>

#include "hashing.h"

namespace std {

template <typename A, typename B>
struct hash<pair<A, B>> {
  std::size_t operator()(const pair<A,B>& s) const noexcept {
    size_t seed = 0;
    HashCombine(seed, A_hasher(s.first));
    HashCombine(seed, B_hasher(s.second));
    return seed;
  }

  hash<A> A_hasher;
  hash<B> B_hasher;
};

}