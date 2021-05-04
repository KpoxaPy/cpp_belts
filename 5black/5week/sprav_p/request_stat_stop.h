#pragma once

#include "request.h"

class StatStopResponse : public Response {
 public:
  StatStopResponse(RequestType type, string name, size_t id, SpravPtr sprav, const Stop* stop);

  Json::Node AsJson() const override;

 private:
  string name_;
  size_t id_ = 0;
  SpravPtr sprav_;
  const Stop* stop_ = nullptr;
};

class StatStopRequest : public Request {
 public:
  StatStopRequest(const Json::Map& dict);

  ResponsePtr Process(SpravPtr sprav) const override;
  Json::Node AsJson() const override;

 private:
  string name_;
  size_t id_ = 0;
};