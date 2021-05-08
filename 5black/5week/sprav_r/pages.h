#pragma once

#include <memory>
#include <unordered_set>

#include "database.pb.h"
#include "database_queries.pb.h"
#include "json.h"

class Pages {
 public:
  using Companies = std::unordered_set<size_t>;

 public:
  Pages() = default;
  Pages(const Json::Dict& dict);
  Pages(const YellowPages::Database& m);
  void Serialize(YellowPages::Database& m);

  const YellowPages::Company& operator[](size_t id) const;

  Companies Process(const YellowPages::Query& query) const;

 private:
  YellowPages::Database db_;

  void ParseFrom(const YellowPages::Database& m);
};
using PagesPtr = std::shared_ptr<Pages>;