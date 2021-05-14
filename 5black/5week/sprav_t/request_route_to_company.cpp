#include "request_route_to_company.h"

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

RouteToCompanyResponse::RouteToCompanyResponse(RequestType type, size_t id, Sprav::Route route, std::string map)
    : Response(type), id_(id), route_(move(route)), map_(map) {
  empty_ = false;
}

Json::Node RouteToCompanyResponse::AsJson() const {
  Json::Dict dict;
  dict["request_id"] = id_;
  if (route_) {
    dict["map"] = map_;
    dict["total_time"] = route_.GetTotalTime();
    dict["items"] = route_.AsJson();
  } else {
    dict["error_message"] = "not found";
  }
  return dict;
}

RouteToCompanyRequest::RouteToCompanyRequest(const Json::Dict& dict)
    : Request(RequestType::ROUTE) {
  id_ = dict.at("id").AsInt();
  from_ = dict.at("from").AsString();

  ostringstream ss;
  Json::PrintNode(dict.at("companies").AsDict(), ss);
  if (auto status = JsonStringToMessage(ss.str(), &query_, JSON_PARSE_OPTIONS); status != Status::OK) {
    ostringstream err_ss;
    err_ss << "Failed to parse " << quoted(ss.str()) << " as find companies request: " << status.ToString();
    throw runtime_error(err_ss.str());
  }
}

ResponsePtr RouteToCompanyRequest::Process(SpravPtr sprav) const {
  auto route = sprav->FindRouteToCompany(from_, query_);
  auto map = sprav->GetRouteMap(route);
  return make_shared<RouteToCompanyResponse>(type_, id_, std::move(route), std::move(map));
}

Json::Node RouteToCompanyRequest::AsJson() const {
  throw runtime_error("unimplemented");
}
