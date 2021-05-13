#pragma once

#include <utility>
#include <string_view>
#include <optional>
#include <stdexcept>

std::pair<std::string_view, std::optional<std::string_view>>
SplitTwoStrict(std::string_view s, std::string_view delimiter = " ");

std::pair<std::string_view, std::string_view>
SplitTwo(std::string_view s, std::string_view delimiter = " ");

std::string_view ReadToken(std::string_view& s, std::string_view delimiter = " ");

template <typename Function>
void TrimLeft(std::string_view& s, Function f) {
  while (!s.empty() && f(s.front())) {
    s.remove_prefix(1);
  }
}

template <typename Function>
void TrimRight(std::string_view& s, Function f) {
  while (!s.empty() && f(s.back())) {
    s.remove_suffix(1);
  }
}

void TrimLeft(std::string_view& s);
void TrimRight(std::string_view& s);
void Trim(std::string_view& s);

template <typename Type>
Type FromString(std::string_view) {
  throw std::runtime_error("Not implemented");
}

template<> double FromString<double>(std::string_view str);
template<> int FromString<int>(std::string_view str);