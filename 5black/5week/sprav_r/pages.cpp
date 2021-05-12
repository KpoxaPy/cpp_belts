#include "pages.h"

#include <google/protobuf/util/json_util.h>

#include <deque>
#include <iomanip>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <functional>

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

void UnionIn(Pages::Companies& lhs, const Pages::Companies* rhs) {
  if (rhs) {
    UnionIn(lhs, *rhs);
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

template <typename Func, typename ValuesContainer>
Pages::Companies
FindOneOfInIndex(const ValuesContainer& values, Func f) {
  Pages::Companies result;
  for (const auto& v : values) {
    UnionIn(result, f(v));
  }
  return result;
}

}  // namespace

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
    auto& c = db_.companies()[id];

    all_companies_.insert(id);

    for (auto r_id : c.rubrics()) {
      rubrics_index_[r_id].insert(id);
    }

    for (auto& n : c.names()) {
      names_index_[n.value()].insert(id);
    }

    for (auto& url : c.urls()) {
      urls_index_[url.value()].insert(id);
    }

    for (auto& phone : c.phones()) {
      AddPhoneToIndex(phone, id);
    }
  }
}

const YellowPages::Company& Pages::operator[](size_t id) const {
  return db_.companies()[id];
}

Pages::Companies Pages::Process(const YellowPages::Query& query) const {
  Companies result = all_companies_;

  if (query.rubrics().size() > 0) {
    deque<size_t> rubrics_ids;
    for (auto& r : query.rubrics()) {
      rubrics_ids.push_back(rubrics_projection_.at(r));
    }
    result = Intersect(result, FindOneOfInIndex(rubrics_ids, [this](size_t value) {
      return FindInIndex(rubrics_index_, value);
    }));
  }

  if (query.names().size() > 0) {
    result = Intersect(result, FindOneOfInIndex(query.names(), [this](const std::string& value) {
      return FindInIndex(names_index_, value);
    }));
  }

  if (query.urls().size() > 0) {
    result = Intersect(result, FindOneOfInIndex(query.urls(), [this](const std::string& value) {
      return FindInIndex(urls_index_, value);
    }));
  }

  if (query.phones().size() > 0) {
    result = Intersect(result, FindOneOfInIndex(query.phones(), [this](const YellowPages::Phone& value) {
      return FindPhoneInIndex(value);
    }));
  }

  return result;
}

void Pages::ParseFrom(const YellowPages::Database& m) {
  db_.CopyFrom(m);
}

void Pages::AddPhoneToIndex(const YellowPages::Phone& m, size_t company_id) {
  phones_number_index_[m.number()].insert(company_id);
  phones_local_code_index_[m.local_code()].insert(company_id);

  if (m.country_code().size() > 0) {
    phones_country_code_index_[m.country_code()].insert(company_id);
  }

  if (m.extension().size() > 0) {
    phones_extension_index_[m.extension()].insert(company_id);
  }

  if (m.type().empty()) {
    phones_type_index_["PHONE"].insert(company_id);
  } else {
    phones_type_index_[m.type()].insert(company_id);
  }
}

Pages::Companies Pages::FindPhoneInIndex(const YellowPages::Phone& m) const {
  Companies result;

  if (auto c = FindInIndex(phones_number_index_, m.number()); c) {
    result = *c;
  }

  if (result.size() > 0 && (m.country_code().size() > 0 || m.local_code().size() > 0)) {
    if (auto c = FindInIndex(phones_local_code_index_, m.local_code()); c) {
      result = Intersect(result, *c);
    }
  }

  if (result.size() > 0 && m.country_code().size() > 0) {
    if (auto c = FindInIndex(phones_country_code_index_, m.country_code()); c) {
      result = Intersect(result, *c);
    }
  }

  if (result.size() > 0 && m.extension().size() > 0) {
    if (auto c = FindInIndex(phones_extension_index_, m.extension()); c) {
      result = Intersect(result, *c);
    }
  }

  if (result.size() > 0 && m.type().size() > 0) {
    if (auto c = FindInIndex(phones_type_index_, m.type()); c) {
      result = Intersect(result, *c);
    }
  }

  return result;
}