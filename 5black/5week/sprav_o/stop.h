#pragma once

#include <string_view>
#include <set>
#include <unordered_map>
#include <ostream>

#include "transport_catalog.pb.h"

struct Stop {
  size_t id;
  std::string_view name;
  double lat;
  double lon;
  std::set<size_t> buses;
  std::unordered_map<size_t, int> distances;

  int DistanceTo(size_t other) const;

  void SerializeTo(SpravSerialize::Stop& m) const;
  static Stop Parse(const SpravSerialize::Stop& m);
};

std::ostream& operator<<(std::ostream& s, const Stop& stop);

double GetDistance(const Stop& lhs, const Stop& rhs);
