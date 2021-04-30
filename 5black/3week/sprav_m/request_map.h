#pragma once

#include "request.h"

class MapResponse : public Response {
 public:
  MapResponse(RequestType type, size_t id, std::string map);

  Json::Node AsJson() const override;

 private:
  size_t id_ = 0;
  std::string map_;
};

class MapRequest : public Request {
 public:
  MapRequest(const Json::Map& dict);

  ResponsePtr Process(SpravPtr sprav) const override;
  Json::Node AsJson() const override;

 private:
  size_t id_ = 0;
};