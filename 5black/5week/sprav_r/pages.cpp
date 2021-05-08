#include "pages.h"

#include <google/protobuf/util/json_util.h>

#include <deque>
#include <iomanip>
#include <optional>
#include <sstream>
#include <stdexcept>

#include "profile.h"

using namespace std;
using ::google::protobuf::util::JsonStringToMessage;
using ::google::protobuf::util::JsonParseOptions;
using ::google::protobuf::util::Status;

namespace {

const JsonParseOptions JSON_PARSE_OPTIONS = [](){
  JsonParseOptions o;
  o.ignore_unknown_fields = false;
  return o;
}();

Pages::Companies Intersect(const Pages::Companies& lhs, const Pages::Companies& rhs) {
  if (lhs.size() > rhs.size()) {
    return Intersect(rhs, lhs);
  }

  Pages::Companies result;
  for (size_t id : lhs) {
    if (rhs.count(id) > 0) {
      result.insert(id);
    }
  }
  return result;
}

void UnionIn(Pages::Companies& lhs, const Pages::Companies& rhs) {
  for (auto& id : rhs) {
    lhs.insert(id);
  }
}

template <typename IndexType>
const Pages::Companies*
FindInIndex(const std::unordered_map<IndexType, Pages::Companies>& index, const IndexType& value) {
  auto it = index.find(value);
  if (it != index.end()) {
    return &it->second;
  } else {
    return nullptr;
  }
}

template <typename IndexType, typename ValuesContainer>
Pages::Companies
FindOneOfInIndex(const std::unordered_map<IndexType, Pages::Companies>& index, const ValuesContainer& values) {
  Pages::Companies result;
  for (const auto& v : values) {
    if (const Pages::Companies* r = FindInIndex(index, v); r) {
      UnionIn(result, *r);
    }
  }
  return result;
}

/* bool DoesPhoneMatch(const YellowPages::Phone& lhs, const YellowPages::Phone& rhs) {
  const Phone& query_phone = query.phone;
  if (!query_phone.extension().empty() && query_phone.extension() != object.extension()) {
    return false;
  }
  if (query.has_type && query_phone.type() != object.type()) {
    return false;
  }
  if (!query_phone.country_code().empty() && query_phone.country_code() != object.country_code()) {
    return false;
  }
  if (
      (!query_phone.local_code().empty() || !query_phone.country_code().empty())
      && query_phone.local_code() != object.local_code()
  ) {
    return false;
  }
  return query_phone.number() == object.number();
} */


} // namespace

Pages::Pages(const Json::Dict& dict) {
  YellowPages::Database db;
  ostringstream ss;
  Json::PrintNode(dict, ss);
  if (auto status = JsonStringToMessage(ss.str(), &db, JSON_PARSE_OPTIONS); status != Status::OK) {
    ostringstream err_ss;
    err_ss << "Failed to parse " << quoted(ss.str()) << " as yellow pages database: " << status.ToString();
    throw runtime_error(err_ss.str());
  }
  ParseFrom(db);
}

Pages::Pages(const YellowPages::Database& m) {
  ParseFrom(m);
  BuildIndex();
}

void Pages::Serialize(YellowPages::Database& m) {
  m.CopyFrom(db_);
}

void Pages::BuildIndex() {
  LOG_DURATION("Pages::BuildIndex");

  for (auto& r : db_.rubrics()) {
    rubrics_projection_[r.second.name()] = r.first;
  }

  size_t companies_size = db_.companies_size();
  for (size_t id = 0; id < companies_size; ++id) {
    auto& c = db_.companies().at(id);

    for (auto r_id : c.rubrics()) {
      rubrics_index_[r_id].insert(id);
    }

    for (auto& n : c.names()) {
      names_index_[n.value()].insert(id);
    }

    for (auto& url : c.urls()) {
      urls_index_[url.value()].insert(id);
    }
  }
}

const YellowPages::Company& Pages::operator[](size_t id) const {
  return db_.companies().at(id);
}

Pages::Companies Pages::Process(const YellowPages::Query& query) const {
  optional<Companies> rubrics_result;
  if (query.rubrics().size() > 0) {
    deque<size_t> rubrics_ids;
    for (auto& r : query.rubrics()) {
      rubrics_ids.push_back(rubrics_projection_.at(r));
    }
    rubrics_result = FindOneOfInIndex(rubrics_index_, rubrics_ids);
  }

  optional<Companies> names_result;
  if (query.names().size() > 0) {
    names_result = FindOneOfInIndex(names_index_, query.names());
  }

  optional<Companies> urls_result;
  if (query.urls().size() > 0) {
    urls_result = FindOneOfInIndex(urls_index_, query.urls());
  }

  Companies result;
  bool first_added = false;

#define INTERSECT_WITH_PARTIAL_RESULT(part)     \
  if (part) {                                   \
    if (!first_added) {                         \
      result = std::move(part.value());         \
      first_added = true;                       \
    } else {                                    \
      result = Intersect(result, part.value()); \
    }                                           \
  }

  INTERSECT_WITH_PARTIAL_RESULT(rubrics_result);
  INTERSECT_WITH_PARTIAL_RESULT(names_result);
  INTERSECT_WITH_PARTIAL_RESULT(urls_result);

#undef INTERSECT_WITH_PARTIAL_RESULT

  return result;
}

void Pages::ParseFrom(const YellowPages::Database& m) {
  db_.CopyFrom(m);
}