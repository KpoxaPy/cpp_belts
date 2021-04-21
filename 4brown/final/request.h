#pragma once

#include <memory>
#include <string_view>

#include "sprav.h"
#include "json.h"

enum class Format {
  CONSOLE,
  JSON_PRETTY,
  JSON
};

enum class RequestType {
  BASE_BUS,
  BASE_STOP,
  STAT_BUS,
  STAT_STOP,
  ROUTE
};

class Response {
 public:
  Response(RequestType type)
    : type_(type) {}
  virtual ~Response() = default;

  virtual std::string AsString() const = 0;
  virtual Json::Node AsJson() const = 0;

  bool empty() const {
    return empty_;
  }

 protected:
  RequestType type_;
  bool empty_ = true;
};
using ResponsePtr = std::shared_ptr<Response>;

class Request {
 public:
  Request(RequestType type)
      : type_(type) {}
  virtual ~Request() = default;

  virtual ResponsePtr Process(SpravPtr sprav) const = 0;
  virtual std::string AsString() const = 0;
  virtual Json::Node AsJson() const = 0;

 protected:
  RequestType type_;
};
using RequestPtr = std::shared_ptr<Request>;

RequestPtr MakeBaseRequest(std::string_view line);
RequestPtr MakeBaseRequest(const Json::Node& doc);
RequestPtr MakeStatRequest(std::string_view line);
RequestPtr MakeStatRequest(const Json::Node& doc);