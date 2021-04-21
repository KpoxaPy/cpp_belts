#pragma once

#include <string_view>
#include <set>
#include <unordered_map>
#include <ostream>

struct Stop {
  size_t id;
  double lat;
  double lon;
  std::set<size_t> buses;
  std::unordered_map<size_t, int> distances;

  bool is_declared = false;

  int DistanceTo(size_t other) const;
};

std::ostream& operator<<(std::ostream& s, const Stop& stop);

double GetDistance(const Stop& lhs, const Stop& rhs);