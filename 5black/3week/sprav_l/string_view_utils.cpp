#include "string_view_utils.h"

#include <cctype>
#include <stdexcept>
#include <sstream>

using namespace std;

pair<string_view, optional<string_view>> SplitTwoStrict(string_view s, string_view delimiter) {
  const size_t pos = s.find(delimiter);
  if (pos == s.npos) {
    return {s, nullopt};
  } else {
    return {s.substr(0, pos), s.substr(pos + delimiter.length())};
  }
}

pair<string_view, string_view> SplitTwo(string_view s, string_view delimiter) {
  const auto [lhs, rhs_opt] = SplitTwoStrict(s, delimiter);
  return {lhs, rhs_opt.value_or("")};
}

string_view ReadToken(string_view& s, string_view delimiter) {
  const auto [lhs, rhs] = SplitTwo(s, delimiter);
  s = rhs;
  return lhs;
}

void TrimLeft(std::string_view& s) {
  while (!s.empty() && isspace(s.front())) {
    s.remove_prefix(1);
  }
}

void TrimRight(std::string_view& s) {
  while (!s.empty() && isspace(s.back())) {
    s.remove_suffix(1);
  }
}

void Trim(std::string_view& s) {
  TrimLeft(s);
  TrimRight(s);
}

template<> double FromString<double>(std::string_view str) {
  size_t pos;
  const double result = stod(string(str), &pos);
  if (pos != str.length()) {
    stringstream error;
    error << "string " << str << " contains " << (str.length() - pos) << " trailing chars";
    throw invalid_argument(error.str());
  }
  return result;
}

template<> int FromString<int>(std::string_view str) {
  size_t pos;
  const int result = stoi(string(str), &pos);
  if (pos != str.length()) {
    std::stringstream error;
    error << "string " << str << " contains " << (str.length() - pos) << " trailing chars";
    throw invalid_argument(error.str());
  }
  return result;
}