#include "request_map.h"

using namespace std;

MapResponse::MapResponse(RequestType type, size_t id, string map)
    : Response(type), id_(id), map_(move(map)) {
  empty_ = false;
}

Json::Node MapResponse::AsJson() const {
  Json::Map dict;
  dict["request_id"] = id_;
  dict["map"] = map_;
  return dict;
}

MapRequest::MapRequest(const Json::Map& dict)
    : Request(RequestType::MAP) {
  id_ = dict.at("id").AsNumber();
}

ResponsePtr MapRequest::Process(SpravPtr sprav) const {
  return make_shared<MapResponse>(type_, id_, sprav->GetMap());
}

Json::Node MapRequest::AsJson() const {
  Json::Map dict;
  return dict;
}