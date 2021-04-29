#include "request_stat_bus.h"

StatBusResponse::StatBusResponse(RequestType type, string name, size_t id, const Bus* bus)
    : Response(type), name_(move(name)), id_(id), bus_(bus) {
  empty_ = false;
}

Json::Node StatBusResponse::AsJson() const {
  Json::Map dict;
  dict["request_id"] = id_;
  if (bus_) {
    dict["route_length"] = bus_->length;
    dict["curvature"] = bus_->curvature;
    dict["stop_count"] = bus_->stops_count;
    dict["unique_stop_count"] = bus_->unique_stops_count;
  } else {
    dict["error_message"] = "not found";
  }
  return dict;
}
StatBusRequest::StatBusRequest(const Json::Map& dict)
    : Request(RequestType::STAT_BUS) {
  name_ = dict.at("name").AsString();
  id_ = dict.at("id").AsNumber();
}

ResponsePtr StatBusRequest::Process(SpravPtr sprav) const {
  return make_shared<StatBusResponse>(type_, name_, id_, sprav->FindBus(name_));
}

Json::Node StatBusRequest::AsJson() const {
  Json::Map dict;
  dict["type"] = "Bus";
  dict["name"] = name_;
  dict["id"] = id_;
  return dict;
}