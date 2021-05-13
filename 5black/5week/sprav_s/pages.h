#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>

#include "database.pb.h"
#include "database_queries.pb.h"
#include "hash_extra.h"
#include "json.h"

class Pages {
 public:
  using Companies = std::unordered_set<size_t>;

 private:
  using RubricsProjection = std::unordered_map<std::string, size_t>;

 public:
  Pages() = default;
  Pages(const Json::Dict& dict);
  Pages(const YellowPages::Database& m);
  void Serialize(YellowPages::Database& m);

  void BuildIndex();

  const YellowPages::Company& operator[](size_t id) const;

  Companies Process(const YellowPages::Query& query) const;

 private:
  YellowPages::Database db_;

  Companies all_companies_;
  RubricsProjection rubrics_projection_;

  void ParseFrom(const YellowPages::Database& m);
};
using PagesPtr = std::shared_ptr<Pages>;