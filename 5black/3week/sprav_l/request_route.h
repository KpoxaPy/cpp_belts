#pragma once

#include "request.h"

class RouteResponse : public Response {
 public:
  RouteResponse(RequestType type, size_t id, Sprav::Route route);

  Json::Node AsJson() const override;

 private:
  size_t id_ = 0;
  Sprav::Route route_;
};

class RouteRequest : public Request {
 public:
  RouteRequest(const Json::Map& dict);

  ResponsePtr Process(SpravPtr sprav) const override;
  Json::Node AsJson() const override;

 private:
  size_t id_ = 0;
  string from_;
  string to_;
};