#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>

#include "database.pb.h"
#include "pages.pb.h"
#include "database_queries.pb.h"
#include "hash_extra.h"
#include "json.h"
#include "working_time.h"

class Pages {
 public:
  using Companies = std::unordered_set<size_t>;

 private:
  using RubricsProjection = std::unordered_map<std::string, size_t>;
  using CompanyWorkingTimes = std::unordered_map<size_t, WorkingTime>;

 public:
  Pages() = default;
  Pages(const Json::Dict& dict);
  Pages(const SpravSerialize::Pages& m);
  void Serialize(SpravSerialize::Pages& m);

  void BuildIndex();

  const YellowPages::Company& operator[](size_t id) const;
  const YellowPages::Company& Get(size_t id) const;
  size_t Size() const;
  const std::string& GetCompanyMainName(size_t id) const;
  std::string GetCompanyFullName(size_t id) const;
  std::optional<double> GetWaitTime(size_t id, const Time& current_time) const;

  Companies Process(const YellowPages::Query& query) const;

 private:
  YellowPages::Database db_;

  RubricsProjection rubrics_projection_;
  CompanyWorkingTimes company_working_times_;

  void ParseFrom(const SpravSerialize::Pages& m);
};
using PagesPtr = std::shared_ptr<Pages>;