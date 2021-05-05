#pragma once

#include "request.h"

class StatStopResponse : public Response {
 public:
  StatStopResponse(RequestType type, string name, size_t id, SpravPtr sprav, const Stop* stop);
  void SetId(size_t id);

  Json::Node AsJson() const override;

 private:
  string name_;
  size_t id_ = 0;
  SpravPtr sprav_;
  const Stop* stop_ = nullptr;
};
using StatStopResponsePtr = std::shared_ptr<StatStopResponse>;

class StatStopRequest : public Request {
 public:
  StatStopRequest(const Json::Map& dict);

  ResponsePtr Process(SpravPtr sprav) const override;
  Json::Node AsJson() const override;

 private:
  string name_;
  size_t id_ = 0;

  static unordered_map<std::string, StatStopResponsePtr> response_cache_;
};