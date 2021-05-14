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

bool Match(const YellowPages::Phone& phone, const YellowPages::Phone& tmpl) {
  if (!tmpl.extension().empty() && tmpl.extension() != phone.extension()) {
    return false;
  }
  if (!tmpl.type().empty()) {
    string type = (phone.type().empty() ? "PHONE" : phone.type());
    if (tmpl.type() != type) {
      return false;
    }
  }
  if (!tmpl.country_code().empty() && tmpl.country_code() != phone.country_code()) {
    return false;
  }
  if (
      (!tmpl.local_code().empty() || !tmpl.country_code().empty())
      && tmpl.local_code() != phone.local_code()
  ) {
    return false;
  }
  return tmpl.number() == phone.number();
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
    company_working_times_.emplace(id, WorkingTime(c.working_time()));
  }
}

const YellowPages::Company& Pages::operator[](size_t id) const {
  return Get(id);
}

const YellowPages::Company& Pages::Get(size_t id) const {
  return db_.companies()[id];
}

size_t Pages::Size() const {
  return db_.companies().size();
}

const std::string& Pages::GetCompanyMainName(size_t id) const {
  auto& c = db_.companies()[id];
  for (auto& n : c.names()) {
    if (n.type() == YellowPages::Name_Type_MAIN) {
      return n.value();
    }
  }

  ostringstream err_ss;
  err_ss << "Could not find MAIN name for company with id=" << id;
  throw runtime_error(err_ss.str());
}

std::string Pages::GetCompanyFullName(size_t id) const {
  auto& c = db_.companies()[id];
  ostringstream ss;
  if (c.rubrics().size() > 0) {
    ss << db_.rubrics().at(c.rubrics()[0]).name() << " ";
  }
  ss << GetCompanyMainName(id);
  return ss.str();
}

std::optional<double> Pages::GetWaitTime(size_t id, const Time& current_time) const {
  return company_working_times_.at(id).GetWaitTime(current_time);
}

Pages::Companies Pages::Process(const YellowPages::Query& query) const {
  deque<size_t> queried_rubrics;
  for (auto& r : query.rubrics()) {
    queried_rubrics.push_back(rubrics_projection_.at(r));
  }

  Companies result;
  size_t companies_size = db_.companies_size();
  for (size_t id = 0; id < companies_size; ++id) {
    auto& c = db_.companies()[id];
    bool ok = true;

    if (ok && query.names().size() > 0) {
      ok = false;
      for (auto& name : c.names()) {
        for (auto& tmpl : query.names()) {
          if (name.value() == tmpl) {
            ok = true;
            break;
          }
        }
        if (ok) {
          break;
        }
      }
    }

    if (ok && query.urls().size() > 0) {
      ok = false;
      for (auto& url : c.urls()) {
        for (auto& tmpl : query.urls()) {
          if (url.value() == tmpl) {
            ok = true;
            break;
          }
        }
        if (ok) {
          break;
        }
      }
    }

    if (ok && query.rubrics().size() > 0) {
      ok = false;
      for (auto& rubric : c.rubrics()) {
        for (auto& tmpl : queried_rubrics) {
          if (rubric == tmpl) {
            ok = true;
            break;
          }
        }
        if (ok) {
          break;
        }
      }
    }

    if (ok && query.phones().size() > 0) {
      ok = false;
      for (auto& phone : c.phones()) {
        for (auto& tmpl : query.phones()) {
          if (Match(phone, tmpl)) {
            ok = true;
            break;
          }
        }
        if (ok) {
          break;
        }
      }
    }

    if (ok) {
      result.insert(id);
    }
  }

  return result;
}

void Pages::ParseFrom(const YellowPages::Database& m) {
  db_.CopyFrom(m);
}
