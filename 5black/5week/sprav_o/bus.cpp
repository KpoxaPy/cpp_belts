#include "bus.h"

using namespace std;

void Bus::SerializeTo(SpravSerialize::Bus& m) const {
  m.set_id(id);
  m.mutable_name()->assign(name);

  m.set_stops_count(stops_count);
  m.set_unique_stops_count(unique_stops_count);
  m.set_length(length);
  m.set_curvature(curvature);
}

Bus Bus::Parse(const SpravSerialize::Bus& m) {
  Bus b;

  b.id = m.id();
  b.stops_count = m.stops_count();
  b.unique_stops_count = m.unique_stops_count();
  b.length = m.length();
  b.curvature = m.curvature();

  return b;
}