#include "routing_settings.h"

using namespace std;

RoutingSettings::RoutingSettings(const Json::Dict& dict) {
  bus_wait_time = dict.at("bus_wait_time").AsInt();
  bus_velocity = static_cast<double>(dict.at("bus_velocity").AsInt()) * 1000 / 60;
}
