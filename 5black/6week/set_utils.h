#pragma once

#include <unordered_set>

template <typename Value>
std::unordered_set<Value> SetDifference(
  const std::unordered_set<Value>& lhs,
  const std::unordered_set<Value>& rhs
) {
  std::unordered_set<Value> result;
  for (const auto& v : lhs) {
    if (rhs.find(v) == rhs.end()) {
      result.insert(v);
    }
  }
  return result;
}

template <typename Value>
std::pair<std::unordered_set<Value>,std::unordered_set<Value>> SetTwoSideDifference(
  const std::unordered_set<Value>& lhs,
  const std::unordered_set<Value>& rhs
) {
  std::unordered_set<Value> lhs_to_rhs;
  std::unordered_set<Value> rhs_to_lhs = rhs;
  for (const auto& v : lhs) {
    if (rhs.find(v) == rhs.end()) {
      lhs_to_rhs.insert(v);
    }
    rhs_to_lhs.erase(v);
  }
  return {std::move(lhs_to_rhs), std::move(rhs_to_lhs)};
}