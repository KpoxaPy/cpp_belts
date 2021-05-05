#pragma once

#include "request.h"

class StatBusResponse : public Response {
 public:
  StatBusResponse(RequestType type, string name, size_t id, const Bus* bus);
  void SetId(size_t id);

  Json::Node AsJson() const override;

 private:
  string name_;
  size_t id_ = 0;
  const Bus* bus_ = nullptr;
};
using StatBusResponsePtr = std::shared_ptr<StatBusResponse>;

class StatBusRequest : public Request {
 public:
  StatBusRequest(const Json::Map& dict);

  ResponsePtr Process(SpravPtr sprav) const override;
  Json::Node AsJson() const override;

 private:
  string name_;
  size_t id_ = 0;

  static unordered_map<std::string, StatBusResponsePtr> response_cache_;
};