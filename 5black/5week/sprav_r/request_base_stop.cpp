#include "request_base_stop.h"

BaseStopRequest::BaseStopRequest(const Json::Dict& dict)
    : Request(RequestType::BASE_STOP) {
  name_ = dict.at("name").AsString();
  lat_ = dict.at("latitude").AsDouble();
  lon_ = dict.at("longitude").AsDouble();
  for (auto& [name, dist] : dict.at("road_distances").AsDict()) {
    distances_[name] = dist.AsInt();
  }
}

ResponsePtr BaseStopRequest::Process(SpravPtr sprav) const {
  sprav->AddStop(name_, lat_, lon_, distances_);
  return make_shared<EmptyResponse>(type_);
}

Json::Node BaseStopRequest::AsJson() const {
  Json::Dict dict;
  dict["type"] = "Stop";
  dict["name"] = name_;
  dict["latitude"] = lat_;
  dict["longitude"] = lon_;
  Json::Dict distances;
  for (const auto& [name, dist] : distances_) {
    distances[string(name)] = dist;
  }
  dict["road_distances"] = move(distances);
  return dict;
}