#pragma once

#include <istream>
#include <ostream>
#include <vector>
#include <future>
#include <shared_mutex>

#include "index.h"

class SearchServer {
public:
  SearchServer() = default;
  explicit SearchServer(std::istream& document_input);
  void UpdateDocumentBase(std::istream& document_input);
  void AddQueriesStream(std::istream& query_input, std::ostream& search_results_output);

private:
  InvertedIndex index;
  std::shared_mutex index_mutex;
  std::vector<std::future<void>> updates_;
  std::vector<std::future<void>> adds_;

  void UpdateDocumentBaseImpl(std::istream& document_input);
  void AddQueriesStreamImpl(std::istream& query_input, std::ostream& search_results_output);
};
