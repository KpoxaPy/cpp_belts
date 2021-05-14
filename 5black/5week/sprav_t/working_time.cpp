#include "working_time.h"

Time::Time(const Json::Node& m) {
  auto& a = m.AsArray();
  day = a[0].AsInt();
  min = a[1].AsInt() * 60 + a[2].AsInt();
}

void Time::Add(double additional_min) {
  uint64_t total_min = min + additional_min;
  if (total_min >= 1440) {
    min = total_min % 1440;
  }
  day = (day + total_min / 1440) % 7;
}

Time Time::operator+(double additional_min) const {
  Time result = *this;
  result.Add(additional_min);
  return result;
}