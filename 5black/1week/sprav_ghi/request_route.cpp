#include "request_route.h"

RouteResponse::RouteResponse(RequestType type, size_t id, Sprav::Route route)
    : Response(type), id_(id), route_(move(route)) {
  empty_ = false;
}

Json::Node RouteResponse::AsJson() const {
  Json::Map dict;
  dict["request_id"] = id_;
  if (route_) {
    dict["total_time"] = route_.GetTotalTime();
    Json::Array items;
    for (auto part : route_) {
      if (part.type == RoutePartType::NOOP) {
        continue;
      }

      Json::Map item_dict;
      if (part.type == RoutePartType::WAIT) {
        item_dict["type"] = "Wait";
        item_dict["time"] = part.time;
        item_dict["stop_name"] = string(part.name);
      } else if (part.type == RoutePartType::BUS) {
        item_dict["type"] = "Bus";
        item_dict["time"] = part.time;
        item_dict["bus"] = string(part.name);
        item_dict["span_count"] = part.span_count;
      }
      items.push_back(move(item_dict));
    }
    dict["items"] = move(items);
  } else {
    dict["error_message"] = "not found";
  }
  return dict;
}

RouteRequest::RouteRequest(const Json::Map& dict)
    : Request(RequestType::ROUTE) {
  id_ = dict.at("id").AsNumber();
  from_ = dict.at("from").AsString();
  to_ = dict.at("to").AsString();
}

ResponsePtr RouteRequest::Process(SpravPtr sprav) const {
  return make_shared<RouteResponse>(type_, id_, sprav->FindRoute(from_, to_));
}

Json::Node RouteRequest::AsJson() const {
  Json::Map dict;
  dict["id"] = id_;
  dict["from"] = from_;
  dict["to"] = to_;
  return dict;
}