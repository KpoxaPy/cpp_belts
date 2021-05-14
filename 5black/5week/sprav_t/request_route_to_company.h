#pragma once

#include "database_queries.pb.h"
#include "request.h"

class RouteToCompanyResponse : public Response {
 public:
  RouteToCompanyResponse(RequestType type, size_t id, Sprav::Route route, std::string map);

  Json::Node AsJson() const override;

 private:
  size_t id_ = 0;
  Sprav::Route route_;
  std::string map_;
};

class RouteToCompanyRequest : public Request {
 public:
  RouteToCompanyRequest(const Json::Dict& dict);

  ResponsePtr Process(SpravPtr sprav) const override;
  Json::Node AsJson() const override;

 private:
  size_t id_ = 0;
  string from_;
  YellowPages::Query query_;
};