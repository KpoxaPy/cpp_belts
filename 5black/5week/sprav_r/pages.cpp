#include "pages.h"

#include <google/protobuf/util/json_util.h>

#include <sstream>
#include <iomanip>
#include <stdexcept>

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
}

void Pages::Serialize(YellowPages::Database& m) {
  m.CopyFrom(db_);
}

const YellowPages::Company& Pages::operator[](size_t id) const {
  return db_.companies().at(id);
}

Pages::Companies Pages::Process(const YellowPages::Query& /* query */) const {
  return {0};
}

void Pages::ParseFrom(const YellowPages::Database& m) {
  db_.CopyFrom(m);
}