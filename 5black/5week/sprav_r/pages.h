#pragma once

#include <memory>

#include "database.pb.h"
#include "json.h"

class Pages {
 public:
  Pages() = default;
  Pages(const Json::Dict& dict);
  Pages(const YellowPages::Database& m);
  void Serialize(YellowPages::Database& m);
};
using PagesPtr = std::shared_ptr<Pages>;