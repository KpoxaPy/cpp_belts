#include "request_base_stop.h"

BaseStopRequest::BaseStopRequest(const Json::Map& dict)
    : Request(RequestType::BASE_STOP) {
  name_ = dict.at("name").AsString();
  lat_ = dict.at("latitude").AsNumber();
  lon_ = dict.at("longitude").AsNumber();
  for (auto& [name, dist] : dict.at("road_distances").AsMap()) {
    distances_[name] = dist.AsNumber();
  }
}

ResponsePtr BaseStopRequest::Process(SpravPtr sprav) const {
  sprav->AddStop(name_, lat_, lon_, distances_);
  return make_shared<EmptyResponse>(type_);
}

Json::Node BaseStopRequest::AsJson() const {
  Json::Map dict;
  dict["type"] = "Stop";
  dict["name"] = name_;
  dict["latitude"] = lat_;
  dict["longitude"] = lon_;
  Json::Map distances;
  for (const auto& [name, dist] : distances_) {
    distances[string(name)] = dist;
  }
  dict["road_distances"] = move(distances);
  return dict;
}