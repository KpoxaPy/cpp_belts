#include "pages.h"

#include <google/protobuf/util/json_util.h>

#include <sstream>

using namespace std;

Pages::Pages(const Json::Dict& dict) {
  YellowPages::Database db;
  ostringstream ss;
  Json::PrintNode(dict, ss);
  ::google::protobuf::util::JsonStringToMessage(ss.str(), &db);
  ParseFrom(db);
}

Pages::Pages(const YellowPages::Database& m) {
  ParseFrom(m);
}

void Pages::Serialize(YellowPages::Database& m) {
  m.CopyFrom(db_);
}

void Pages::ParseFrom(const YellowPages::Database& m) {
  db_.CopyFrom(m);
}