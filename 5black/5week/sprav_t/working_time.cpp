#include "working_time.h"

#include <cmath>
#include <tuple>

using namespace std;;

Time Time::From(const Json::Node& m) {
  auto& a = m.AsArray();
  uint8_t day = a[0].AsInt();
  double min = a[1].AsInt() * 60 + a[2].AsInt();
  return {Type::NONE, day, min};
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

double Time::operator-(const Time& other) const {
  int days = day - other.day;
  double mins = min - other.min;
  return days * 1440.0 + mins;
}

bool Time::operator==(const Time& other) const {
  return tie(day, min) == tie(other.day, other.min);
}

bool Time::operator<(const Time& other) const {
  return tie(day, min) < tie(other.day, other.min);
}

WorkingTime::WorkingTime(const YellowPages::WorkingTime& m) {
  for (auto& interval : m.intervals()) {
    if (interval.day() == YellowPages::WorkingTimeInterval_Day_EVERYDAY) {
      for (uint8_t day = 0; day < 7; ++day) {
        intervals_.insert({Time::Type::FROM, day, static_cast<double>(interval.minutes_from())});
        intervals_.insert({Time::Type::TO, day, static_cast<double>(interval.minutes_to())});
      }
    } else {
      const uint8_t day = static_cast<uint8_t>(interval.day());
      intervals_.insert({Time::Type::FROM, day, static_cast<double>(interval.minutes_from())});
      intervals_.insert({Time::Type::TO, day, static_cast<double>(interval.minutes_to())});
    }
  }
}

std::optional<double> WorkingTime::GetWaitTime(const Time& current_time) const {
  if (intervals_.empty()) {
    return {};
  }

  auto [l_bound, u_bound] = intervals_.equal_range(current_time);

  if (u_bound == intervals_.end() || l_bound == intervals_.end()) {
    return 7*1440 + (*intervals_.begin() - current_time);
  }

  if (l_bound->type == Time::Type::FROM && current_time < *l_bound) {
    return *l_bound - current_time;
  }

  if (l_bound->type == Time::Type::TO && current_time == *l_bound) {
    return *u_bound - current_time;
  }

  return {};
}