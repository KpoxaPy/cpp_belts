#pragma once

#include "request.h"

class BaseStopRequest : public Request {
 public:
  BaseStopRequest(const Json::Dict& dict);

  ResponsePtr Process(SpravPtr sprav) const override;
  Json::Node AsJson() const override;

 private:
  string name_;
  double lat_;
  double lon_;
  unordered_map<string, int> distances_;
};