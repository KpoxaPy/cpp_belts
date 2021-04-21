#include "routing_settings.h"

using namespace std;

RoutingSettings::RoutingSettings(const Json::Map& dict) {
  bus_wait_time = dict.at("bus_wait_time").AsNumber();
  bus_velocity = static_cast<double>(dict.at("bus_velocity").AsNumber()) * 1000 / 60;
}
