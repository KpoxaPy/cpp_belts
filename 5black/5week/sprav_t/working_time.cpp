#include "working_time.h"

#include <cmath>

Time::Time(const Json::Node& m) {
  auto& a = m.AsArray();
  day = a[0].AsInt();
  min = a[1].AsInt() * 60 + a[2].AsInt();
}

void Time::Add(double additional_min) {
  const double total_min = min + additional_min;
  min = fmod(total_min, 1440);
  day = (day + static_cast<int64_t>(floor(total_min / 1440))) % 7;
}

Time Time::operator+(double additional_min) const {
  Time result = *this;
  result.Add(additional_min);
  return result;
}