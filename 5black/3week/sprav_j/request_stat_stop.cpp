#include "request_stat_stop.h"

StatStopResponse::StatStopResponse(RequestType type, string name, size_t id, SpravPtr sprav, const Stop* stop)
    : Response(type), name_(move(name)), id_(id), sprav_(sprav), stop_(stop) {
  empty_ = false;
}

Json::Node StatStopResponse::AsJson() const {
  Json::Map dict;
  dict["request_id"] = id_;
  if (stop_) {
    set<string_view> buses_views;
    for (const auto& bus_id : stop_->buses) {
      buses_views.emplace(sprav_->GetBus(bus_id).name);
    }

    Json::Array buses;
    for (const auto& b : buses_views) {
      buses.emplace_back(string(b));
    }
    dict["buses"] = move(buses);
  } else {
    dict["error_message"] = "not found";
  }
  return dict;
}

StatStopRequest::StatStopRequest(const Json::Map& dict)
    : Request(RequestType::STAT_STOP) {
  name_ = dict.at("name").AsString();
  id_ = dict.at("id").AsNumber();
}

ResponsePtr StatStopRequest::Process(SpravPtr sprav) const {
  return make_shared<StatStopResponse>(type_, name_, id_, sprav, sprav->FindStop(name_));
}

Json::Node StatStopRequest::AsJson() const {
  Json::Map dict;
  dict["type"] = "Stop";
  dict["name"] = name_;
  dict["id"] = id_;
  return dict;
}