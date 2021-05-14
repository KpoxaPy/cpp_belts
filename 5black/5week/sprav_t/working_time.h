#pragma once

#include <set>
#include <optional>

#include "json.h"
#include "working_time.pb.h"

struct Time {
  enum class Type {
    NONE,
    FROM,
    TO
  };

  static Time From(const Json::Node& m);

  void Add(double additional_min);

  Time operator+(double additional_min) const;
  double operator-(const Time& other) const;
  bool operator==(const Time& other) const;
  bool operator<(const Time& other) const;

  Type type = Type::NONE;
  uint8_t day = 0;
  double min = 0; 
};

class WorkingTime {
 public:
  WorkingTime() = default;
  WorkingTime(const YellowPages::WorkingTime& m);

  std::optional<double> GetWaitTime(const Time& current_time) const;

 private:
  std::set<Time> intervals_;
};