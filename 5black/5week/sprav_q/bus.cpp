#include "bus.h"

using namespace std;

void Bus::SerializeTo(SpravSerialize::Bus& m) const {
  m.set_id(id);
  m.mutable_name()->assign(name);
  for (auto stop_id : stops) {
    m.add_stop_id(stop_id);
  }
  m.set_is_roundtrip(is_roundtrip);

  m.set_stops_count(stops_count);
  m.set_unique_stops_count(unique_stops_count);
  m.set_length(length);
  m.set_curvature(curvature);
}

Bus Bus::Parse(const SpravSerialize::Bus& m) {
  Bus b;

  b.id = m.id();
  for (auto stop_id: m.stop_id()) {
    b.stops.push_back(stop_id);
  }
  b.is_roundtrip = m.is_roundtrip();

  b.stops_count = m.stops_count();
  b.unique_stops_count = m.unique_stops_count();
  b.length = m.length();
  b.curvature = m.curvature();

  return b;
}