#pragma once

#include <ostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <deque>
#include <tuple>

#include "profile.h"

using WordIndex = std::vector<std::pair<std::size_t, std::size_t>>;

using SearchTimers = std::tuple<uint64_t&, uint64_t&, uint64_t&, uint64_t&>;

class InvertedIndex {
public:
  std::tuple<uint64_t, uint64_t> Add(std::string&& document);
  const WordIndex& Lookup(std::string_view word) const;
  const std::string& GetDocument(std::size_t id) const;
  void Search(std::string_view query, SearchTimers& ts, WordIndex& result) const;

private:
  std::unordered_map<std::string_view, WordIndex> index;
  std::deque<std::string> docs;

  void InitIndexBuffer(WordIndex& buf) const;
};
