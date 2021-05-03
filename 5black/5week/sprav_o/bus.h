#pragma once

#include <string_view>
#include <list>

struct Bus {
  size_t id;
  std::string_view name;
  std::list<size_t> stops;
  bool is_roundtrip;

  bool has_stats = false;
  size_t stops_count;
  size_t unique_stops_count;
  double length;
  double curvature;
};