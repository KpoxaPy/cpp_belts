#include "request_find.h"

#include <google/protobuf/util/json_util.h>

#include <iomanip>
#include <sstream>
#include <stdexcept>

using namespace std;
using ::google::protobuf::util::JsonStringToMessage;
using ::google::protobuf::util::JsonParseOptions;
using ::google::protobuf::util::Status;

namespace {

const JsonParseOptions JSON_PARSE_OPTIONS = [](){
  JsonParseOptions o;
  o.ignore_unknown_fields = true;
  return o;
}();

} // namespace

FindResponse::FindResponse(RequestType type, size_t id, SpravPtr sprav, Pages::Companies result)
    : Response(type)
    , id_(id)
    , sprav_(sprav)
    , result_(std::move(result))
{
  empty_ = false;
}

Json::Node FindResponse::AsJson() const {
  Json::Array companies;
  companies.reserve(result_.size());
  for (auto id : result_) {
    companies.push_back(sprav_->GetPages()->GetCompanyMainName(id));
  }

  return Json::Dict{
    {"request_id", id_},
    {"companies", std::move(companies)}
  };
}

FindRequest::FindRequest(const Json::Dict& dict)
    : Request(RequestType::FIND_COMPANIES) {
  id_ = dict.at("id").AsInt();
  ostringstream ss;
  Json::PrintNode(dict, ss);
  if (auto status = JsonStringToMessage(ss.str(), &query_, JSON_PARSE_OPTIONS); status != Status::OK) {
    ostringstream err_ss;
    err_ss << "Failed to parse " << quoted(ss.str()) << " as find companies request: " << status.ToString();
    throw runtime_error(err_ss.str());
  }
}

ResponsePtr FindRequest::Process(SpravPtr sprav) const {
  return make_shared<FindResponse>(type_, id_, sprav, sprav->FindCompanies(query_));
}

Json::Node FindRequest::AsJson() const {
  Json::Dict dict;
  return dict;
}