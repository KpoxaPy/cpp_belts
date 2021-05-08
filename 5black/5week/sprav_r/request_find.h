#pragma once

#include "database_queries.pb.h"
#include "request.h"

class FindResponse : public Response {
 public:
  FindResponse(RequestType type, size_t id, SpravPtr sprav);
  void SetId(size_t id);

  Json::Node AsJson() const override;

 private:
  size_t id_ = 0;
  SpravPtr sprav_;
};
using FindResponsePtr = std::shared_ptr<FindResponse>;

class FindRequest : public Request {
 public:
  FindRequest(const Json::Dict& dict);

  ResponsePtr Process(SpravPtr sprav) const override;
  Json::Node AsJson() const override;

 private:
  size_t id_ = 0;
  YellowPages::Query query_;
};