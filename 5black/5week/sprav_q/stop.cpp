#include "stop.h"

#include <cmath>

#include "transport_catalog.pb.h"

using namespace std;

namespace {

const long double EARTH_RADIUS_M = 6371000;
const long double PI = 3.1415926535;

double grad_to_rad(double v) {
  return v / 180 * PI;
}

}

ostream& operator<<(ostream& s, const Stop& stop) {
  return s << "Stop{id=" << stop.id << "}"; 
}

int Stop::DistanceTo(size_t other) const {
  return distances.at(other);
}

void Stop::SerializeTo(SpravSerialize::Stop& m) const {
  m.set_id(id);
  m.set_latitude(lat);
  m.set_longitude(lon);
  m.mutable_name()->assign(name);
  for (auto bus : buses) {
    m.mutable_bus()->Add(bus);
  }
}

Stop Stop::Parse(const SpravSerialize::Stop& m) {
  Stop s;
  s.id = m.id();
  s.lat = m.latitude();
  s.lon = m.longitude();
  s.buses.insert(m.bus().begin(), m.bus().end());

  return s;
}

double GetDistance(const Stop& a, const Stop& b) {
  return
    acos(
      sin(grad_to_rad(a.lat)) * sin(grad_to_rad(b.lat))
      + cos(grad_to_rad(a.lat)) * cos(grad_to_rad(b.lat)) * cos(grad_to_rad(abs(a.lon - b.lon))))
    * EARTH_RADIUS_M;
}