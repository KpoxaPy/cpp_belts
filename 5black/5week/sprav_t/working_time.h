#pragma once

#include "json.h"

struct Time {
  Time() = default;
  Time(const Json::Node& m);

  void Add(double additional_min);

  Time operator+(double additional_min) const;

  uint8_t day = 0;
  double min = 0; 
};