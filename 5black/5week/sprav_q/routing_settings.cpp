#include "routing_settings.h"

using namespace std;

RoutingSettings::RoutingSettings(const Json::Dict& dict) {
  bus_wait_time = dict.at("bus_wait_time").AsDouble();
  bus_velocity = static_cast<double>(dict.at("bus_velocity").AsDouble()) * 1000 / 60;
}
