#include "request.h"
#include "string_view_utils.h"

#include <stdexcept>
#include <sstream>
#include <list>
#include <unordered_map>
#include <iomanip>
#include <iostream>

using namespace std;

namespace {

class EmptyResponse : public Response {
 public:
  EmptyResponse(RequestType type)
    : Response(type)
  {}

  std::string AsString() const override {
    return {};
  }

  Json::Node AsJson() const override {
    return Json::Map{};
  }
};

class BaseBusRequest : public Request {
 public:
  BaseBusRequest(string_view name, string_view line)
    : Request(RequestType::BASE_BUS)
    , name_(name)
  {
    is_ring_route_ = line.find('>') != string_view::npos;

    while (!line.empty()) {
      auto stop = ReadToken(line, is_ring_route_ ? ">" : "-");
      Trim(stop);
      stops_.emplace_back(stop);
      TrimLeft(line);
    }
  }

  BaseBusRequest(const Json::Map& dict)
    : Request(RequestType::BASE_BUS)
  {
    name_ = dict.at("name").AsString();
    auto rt_it = dict.find("is_roundtrip");
    is_ring_route_ = rt_it != dict.end() ? rt_it->second.AsBool() : false;
    for (auto stop : dict.at("stops").AsArray()) {
      stops_.emplace_back(stop.AsString());
    }
  }

  ResponsePtr Process(SpravPtr sprav) const override {
    sprav->AddBus(name_, stops_.begin(), stops_.end(), is_ring_route_);
    return make_shared<EmptyResponse>(type_);
  }

  std::string AsString() const override {
    ostringstream s;
    s << "[Bus " << name_ << ": ";
    bool was_first = false;
    for (auto stop: stops_) {
      s << (was_first ? is_ring_route_ ? " > " : " - " : (was_first = true, "")) << stop;
    }
    return s.str();
  }

  Json::Node AsJson() const override {
    Json::Map dict;
    dict["type"] = "Bus";
    dict["name"] = name_;
    dict["is_roundtrip"] = is_ring_route_;
    Json::Array stops;
    for (const auto& stop: stops_) {
      stops.emplace_back(stop);
    } 
    dict["stops"] = move(stops);
    return dict;
  }

 private:
  string name_;
  list<string> stops_;
  bool is_ring_route_;
};

class BaseStopRequest : public Request {
 public:
  BaseStopRequest(string_view name, string_view line)
    : Request(RequestType::BASE_STOP)
    , name_(name)
  {
    lat_ = FromString<double>(ReadToken(line, ","));
    TrimLeft(line);
    lon_ = FromString<double>(ReadToken(line, ","));
    TrimLeft(line);

    while (!line.empty()) {
      auto dist = FromString<int>(ReadToken(line, "m to"));
      TrimLeft(line);
      auto other_name = ReadToken(line, ",");
      TrimLeft(line);
      distances_.emplace(other_name, dist);
    }
  }

  BaseStopRequest(const Json::Map& dict)
    : Request(RequestType::BASE_STOP)
  {
    name_ = dict.at("name").AsString();
    lat_ = dict.at("latitude").AsNumber();
    lon_ = dict.at("longitude").AsNumber();
    for (auto [name, dist] : dict.at("road_distances").AsMap()) {
      distances_[name] = dist.AsNumber();
    }
  }

  ResponsePtr Process(SpravPtr sprav) const override {
    sprav->AddStop(name_, lat_, lon_, distances_);
    return make_shared<EmptyResponse>(type_);
  }

  std::string AsString() const override {
    ostringstream s;
    s << "Stop " << name_ << ": " << lat_ << ", " << lon_;
    for (auto [other_name, dist]: distances_) {
      s << ", " << dist << "m to " << other_name;
    }
    return s.str();
  }

  Json::Node AsJson() const override {
    Json::Map dict;
    dict["type"] = "Stop";
    dict["name"] = name_;
    dict["latitude"] = lat_;
    dict["longitude"] = lon_;
    Json::Map distances;
    for (const auto& [name, dist]: distances_) {
      distances[string(name)] = dist;
    } 
    dict["road_distances"] = move(distances);
    return dict;
  }

 private:
  string name_;
  double lat_;
  double lon_;
  unordered_map<string, int> distances_;
};

class StatBusResponse : public Response {
 public:
  StatBusResponse(RequestType type, string name, size_t id, const Bus* bus)
    : Response(type)
    , name_(move(name))
    , id_(id)
    , bus_(bus)
  {
    empty_ = false;
  }

  std::string AsString() const override {
    ostringstream s;
    if (bus_) {
      s << "Bus " << name_ << ": "
        << bus_->stops_count << " stops on route, "
        << bus_->unique_stops_count << " unique stops, "
        << setprecision(6) << bus_->length << " route length, "
        << setprecision(7) << bus_->curvature << " curvature";
    } else {
      s << "Bus " << name_ << ": not found";
    }
    return s.str();
  }

  Json::Node AsJson() const override {
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

 private:
  string name_;
  size_t id_ = 0;
  const Bus* bus_ = nullptr;
};

class StatBusRequest : public Request {
 public:
  StatBusRequest(string_view name)
    : Request(RequestType::STAT_BUS)
    , name_(name)
  {
  }

  StatBusRequest(const Json::Map& dict)
    : Request(RequestType::STAT_BUS)
  {
    name_ = dict.at("name").AsString();
    id_ = dict.at("id").AsNumber();
  }

  ResponsePtr Process(SpravPtr sprav) const override {
    return make_shared<StatBusResponse>(type_, name_, id_, sprav->FindBus(name_));
  }

  std::string AsString() const override {
    ostringstream s;
    s << "Bus " << name_;
    return s.str();
  }

  Json::Node AsJson() const override {
    Json::Map dict;
    dict["type"] = "Bus";
    dict["name"] = name_;
    dict["id"] = id_;
    return dict;
  }

 private:
  string name_;
  size_t id_ = 0;
};

class StatStopResponse : public Response {
 public:
  StatStopResponse(RequestType type, string name, size_t id, SpravPtr sprav, const Stop* stop)
    : Response(type)
    , name_(move(name))
    , id_(id)
    , sprav_(sprav)
    , stop_(stop)
  {
    empty_ = false;
  }

  std::string AsString() const override {
    ostringstream s;
    if (stop_) {
      if (stop_->buses.empty()) {
        s << "Stop " << name_ << ": no buses";
      } else {
        s << "Stop " << name_ << ": buses";
        for (const auto& b : stop_->buses) {
          s << " " << sprav_->GetBusName(b);
        }
      }
    } else {
      s << "Stop " << name_ << ": not found";
    }
    return s.str();
  }

  Json::Node AsJson() const override {
    Json::Map dict;
    dict["request_id"] = id_;
    if (stop_) {
      set<string_view> buses_views;
      for (const auto& b : stop_->buses) {
        buses_views.emplace(sprav_->GetBusName(b));
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

 private:
  string name_;
  size_t id_ = 0;
  SpravPtr sprav_;
  const Stop* stop_ = nullptr;
};

class StatStopRequest : public Request {
 public:
  StatStopRequest(string_view name)
    : Request(RequestType::STAT_STOP)
    , name_(name)
  {
  }

  StatStopRequest(const Json::Map& dict)
    : Request(RequestType::STAT_STOP)
  {
    name_ = dict.at("name").AsString();
    id_ = dict.at("id").AsNumber();
  }

  ResponsePtr Process(SpravPtr sprav) const override {
    return make_shared<StatStopResponse>(type_, name_, id_, sprav, sprav->FindStop(name_));
  }

  std::string AsString() const override {
    ostringstream s;
    s << "Stop " << name_;
    return s.str();
  }

  Json::Node AsJson() const override {
    Json::Map dict;
    dict["type"] = "Stop";
    dict["name"] = name_;
    dict["id"] = id_;
    return dict;
  }

 private:
  string name_;
  size_t id_ = 0;
};

class RouteResponse : public Response {
 public:
  RouteResponse(RequestType type, size_t id, Sprav::Route route)
    : Response(type)
    , id_(id)
    , route_(move(route))
  {
    empty_ = false;
  }

  std::string AsString() const override {
    ostringstream s;
    return s.str();
  }

  Json::Node AsJson() const override {
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

 private:
  size_t id_ = 0;
  Sprav::Route route_;
};

class RouteRequest : public Request {
 public:
  RouteRequest(const Json::Map& dict)
    : Request(RequestType::ROUTE)
  {
    id_ = dict.at("id").AsNumber();
    from_ = dict.at("from").AsString();
    to_ = dict.at("to").AsString();
  }

  ResponsePtr Process(SpravPtr sprav) const override {
    return make_shared<RouteResponse>(type_, id_, sprav->FindRoute(from_, to_));
  }

  std::string AsString() const override {
    ostringstream s;
    return s.str();
  }

  Json::Node AsJson() const override {
    Json::Map dict;
    dict["id"] = id_;
    dict["from"] = from_;
    dict["to"] = to_;
    return dict;
  }

 private:
  size_t id_ = 0;
  string from_;
  string to_;
};

}

RequestPtr MakeBaseRequest(std::string_view line) {
  string_view v(line);
  TrimLeft(v);
  auto cmd = ReadToken(v);
  TrimLeft(v);
  auto name = ReadToken(v, ":");
  TrimLeft(v);
  if (cmd == "Bus") {
    return make_shared<BaseBusRequest>(name, v);
  } else if (cmd == "Stop") {
    return make_shared<BaseStopRequest>(name, v);
  }
  throw invalid_argument(string(line));
}

RequestPtr MakeBaseRequest(const Json::Node& doc) {
  // cerr << "MakeBaseRequest from " << doc << endl;

  auto dict = doc.AsMap();
  auto type = dict.at("type").AsString(); 
  if (type == "Bus") {
    return make_shared<BaseBusRequest>(dict);
  } else if (type == "Stop") {
    return make_shared<BaseStopRequest>(dict);
  }
  throw invalid_argument("");
}

RequestPtr MakeStatRequest(std::string_view line) {
  string_view v(line);
  TrimLeft(v);
  auto cmd = ReadToken(v);
  Trim(v);
  auto name = v;

  if (cmd == "Bus") {
    return make_shared<StatBusRequest>(name);
  } else if (cmd == "Stop") {
    return make_shared<StatStopRequest>(name);
  }
  throw invalid_argument(string(line));
}

RequestPtr MakeStatRequest(const Json::Node& doc) {
  // cerr << "MakeStatRequest from " << doc << endl;

  auto dict = doc.AsMap();
  auto type = dict.at("type").AsString(); 
  if (type == "Bus") {
    return make_shared<StatBusRequest>(dict);
  } else if (type == "Stop") {
    return make_shared<StatStopRequest>(dict);
  } else if (type == "Route") {
    return make_shared<RouteRequest>(dict);
  }
  throw invalid_argument("");
}