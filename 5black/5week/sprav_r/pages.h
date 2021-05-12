#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>

#include "database.pb.h"
#include "database_queries.pb.h"
#include "json.h"

class Pages {
 public:
  using Companies = std::unordered_set<size_t>;

 private:
  using RubricsProjection = std::unordered_map<std::string, size_t>;

  using RubricsIndex = std::unordered_map<size_t, Companies>;
  using NamesIndex = std::unordered_map<std::string, Companies>;
  using UrlsIndex = std::unordered_map<std::string, Companies>;
  using PhonesPartIndex = std::unordered_map<std::string, Companies>;

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

  RubricsIndex rubrics_index_;
  NamesIndex names_index_;
  UrlsIndex urls_index_;

  PhonesPartIndex phones_number_index_;
  PhonesPartIndex phones_local_code_index_;
  PhonesPartIndex phones_country_code_index_;
  PhonesPartIndex phones_extension_index_;
  PhonesPartIndex phones_type_index_;

  void ParseFrom(const YellowPages::Database& m);

  void AddPhoneToIndex(const YellowPages::Phone& m, size_t company_id);

  Companies FindPhoneInIndex(const YellowPages::Phone& m) const;

  template <typename ValuesContainer>
  Companies FindPhonesInIndex(const ValuesContainer& values) const;
};
using PagesPtr = std::shared_ptr<Pages>;