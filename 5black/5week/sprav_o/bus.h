#pragma once

#include <string_view>
#include <list>

#include "transport_catalog.pb.h"

struct Bus {
  size_t id;
  std::string_view name;
  std::list<size_t> stops;
  bool is_roundtrip;

  size_t stops_count;
  size_t unique_stops_count;
  size_t length;
  double curvature;

  void SerializeTo(SpravSerialize::Bus& m) const;
  static Bus Parse(const SpravSerialize::Bus& m);
};