#pragma once

#include "request.h"

class BaseBusRequest : public Request {
 public:
  BaseBusRequest(const Json::Dict& dict);

  ResponsePtr Process(SpravPtr sprav) const override;
  Json::Node AsJson() const override;

 private:
  string name_;
  list<string> stops_;
  bool is_ring_route_;
};