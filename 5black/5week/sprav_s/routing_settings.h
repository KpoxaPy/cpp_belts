#pragma once

#include "json.h"

struct RoutingSettings {
  RoutingSettings() = default;
  RoutingSettings(const Json::Dict& dict);

  double bus_wait_time = 1;
  double bus_velocity = 1;
  double pedestrian_velocity = 1;
};