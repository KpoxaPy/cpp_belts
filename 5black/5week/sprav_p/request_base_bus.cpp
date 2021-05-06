#include "request_base_bus.h"

BaseBusRequest::BaseBusRequest(const Json::Dict& dict)
    : Request(RequestType::BASE_BUS) {
  name_ = dict.at("name").AsString();
  auto rt_it = dict.find("is_roundtrip");
  is_ring_route_ = rt_it != dict.end() ? rt_it->second.AsBool() : false;
  for (auto& stop : dict.at("stops").AsArray()) {
    stops_.emplace_back(stop.AsString());
  }
}

ResponsePtr BaseBusRequest::Process(SpravPtr sprav) const {
  sprav->AddBus(name_, stops_, is_ring_route_);
  return make_shared<EmptyResponse>(type_);
}

Json::Node BaseBusRequest::AsJson() const {
  Json::Dict dict;
  dict["type"] = "Bus";
  dict["name"] = name_;
  dict["is_roundtrip"] = is_ring_route_;
  Json::Array stops;
  for (const auto& stop : stops_) {
    stops.emplace_back(stop);
  }
  dict["stops"] = move(stops);
  return dict;
}