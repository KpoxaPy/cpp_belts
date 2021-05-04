#include "request.h"

#include <memory>
#include <stdexcept>

#include "request_base_bus.h"
#include "request_base_stop.h"
#include "request_map.h"
#include "request_route.h"
#include "request_stat_bus.h"
#include "request_stat_stop.h"
#include "string_view_utils.h"

using namespace std;

RequestPtr MakeBaseRequest(const Json::Node& doc) {
  auto dict = doc.AsMap();
  auto type = dict.at("type").AsString(); 
  if (type == "Bus") {
    return make_shared<BaseBusRequest>(dict);
  } else if (type == "Stop") {
    return make_shared<BaseStopRequest>(dict);
  }
  throw invalid_argument("");
}

RequestPtr MakeRequest(const Json::Node& doc) {
  auto dict = doc.AsMap();
  auto type = dict.at("type").AsString(); 
  if (type == "Bus") {
    return make_shared<StatBusRequest>(dict);
  } else if (type == "Stop") {
    return make_shared<StatStopRequest>(dict);
  } else if (type == "Route") {
    return make_shared<RouteRequest>(dict);
  } else if (type == "Map") {
    return make_shared<MapRequest>(dict);
  }
  throw invalid_argument("");
}