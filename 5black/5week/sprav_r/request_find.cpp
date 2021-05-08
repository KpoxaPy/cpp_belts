#include "request_find.h"

using namespace std;

FindResponse::FindResponse(RequestType type, size_t id, SpravPtr sprav)
    : Response(type), id_(id), sprav_(sprav) {
  empty_ = false;
}

Json::Node FindResponse::AsJson() const {
  Json::Dict dict;
  dict["request_id"] = id_;
  return dict;
}

FindRequest::FindRequest(const Json::Dict& dict)
    : Request(RequestType::FIND_COMPANIES) {
  id_ = dict.at("id").AsInt();
}

ResponsePtr FindRequest::Process(SpravPtr sprav) const {
  return make_shared<FindResponse>(type_, id_, sprav);
}

Json::Node FindRequest::AsJson() const {
  Json::Dict dict;
  return dict;
}