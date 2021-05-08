#include "request_stat_bus.h"

unordered_map<std::string, StatBusResponsePtr> StatBusRequest::response_cache_;

StatBusResponse::StatBusResponse(RequestType type, string name, size_t id, const Bus* bus)
    : Response(type), name_(move(name)), id_(id), bus_(bus) {
  empty_ = false;
}

void StatBusResponse::SetId(size_t id) {
  id_ = id;
}

Json::Node StatBusResponse::AsJson() const {
  Json::Dict dict;
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
StatBusRequest::StatBusRequest(const Json::Dict& dict)
    : Request(RequestType::STAT_BUS) {
  name_ = dict.at("name").AsString();
  id_ = dict.at("id").AsInt();
}

ResponsePtr StatBusRequest::Process(SpravPtr sprav) const {
  StatBusResponsePtr resp_ptr;

  if (auto it = response_cache_.find(name_); it != response_cache_.end()) {
    resp_ptr = it->second;
    resp_ptr->SetId(id_);
  } else {
    resp_ptr = make_shared<StatBusResponse>(type_, name_, id_, sprav->FindBus(name_));
    response_cache_[name_] = resp_ptr;
  }

  return resp_ptr;
}

Json::Node StatBusRequest::AsJson() const {
  Json::Dict dict;
  dict["type"] = "Bus";
  dict["name"] = name_;
  dict["id"] = id_;
  return dict;
}