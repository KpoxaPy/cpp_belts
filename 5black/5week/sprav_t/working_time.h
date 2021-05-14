#pragma once

#include <optional>
#include <vector>

#include "json.h"
#include "pages.pb.h"
#include "working_time.pb.h"

struct Time {
  enum class Type {
    NONE,
    FROM,
    TO
  };

  static Time From(const Json::Node& m);
  static Time From(const SpravSerialize::WorkingTime::Time& m);
  void Serialize(SpravSerialize::WorkingTime::Time& m);

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
  WorkingTime(const SpravSerialize::WorkingTime& m);

  void Serialize(SpravSerialize::WorkingTime& m);

  std::optional<double> GetWaitTime(const Time& current_time) const;

 private:
  std::vector<Time> intervals_;
};