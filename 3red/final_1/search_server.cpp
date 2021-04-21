#include "search_server.h"
#include "iterator_range.h"
#include "string_util.h"
#include "search_result.h"
#include "profile.h"

#include <sstream>
#include <iostream>
#include <functional>

using namespace std;

SearchServer::SearchServer(istream& document_input) {
  UpdateDocumentBaseImpl(document_input);
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
  updates_.push_back(async(&SearchServer::UpdateDocumentBaseImpl, this, ref(document_input)));
}

void SearchServer::AddQueriesStream(istream& query_input, ostream& search_results_output) {
  adds_.push_back(async(&SearchServer::AddQueriesStreamImpl, this, ref(query_input), ref(search_results_output)));
}

void SearchServer::UpdateDocumentBaseImpl(istream& document_input) {
  InvertedIndex new_index;

  DurationMeter<microseconds> m;
  uint64_t sum = 0; 
  uint64_t stat = 0; 
  uint64_t push = 0; 
  size_t count = 0;
  for (string current_document; getline(document_input, current_document); ) {
    uint64_t stat_time, push_time;

    m.Start();
    tie(stat_time, push_time) = new_index.Add(move(current_document));
    auto t = m.Get();

    stat += stat_time;
    push += push_time;
    sum += t;
    ++count;
  }
  cerr << "Avg index (" << count << ") total=" << sum / count << "us "
    << "split=" << stat / count << "us "
    << "push=" << push / count << "us "
    << endl;

  lock_guard g(index_mutex);
  index = move(new_index);
}

void SearchServer::AddQueriesStreamImpl(
  istream& query_input, ostream& search_results_output
) {
  WordIndex idx;

  DurationMeter<microseconds> m;
  uint64_t sum = 0;
  uint64_t lookup = 0;
  uint64_t merge = 0;
  uint64_t init_buffer = 0;
  uint64_t merge_1 = 0;
  uint64_t sort = 0;
  SearchTimers ts = tie(lookup, merge, init_buffer, merge_1);
  size_t count = 0;

  for (string current_query; getline(query_input, current_query); ) {
    m.Start();
    {
      shared_lock l(index_mutex);
      index.Search(current_query, ts, idx);
    }
    search_results_output << SearchResult{current_query, idx, sort};
    auto t = m.Get();

    sum += t;
    ++count;
  }

  cerr << "Avg query (" << count << ") total=" << sum / count << "us "
    << "lookup=" << lookup / count << "us "
    << "merge=" << merge / count << "us "
    << "merge_1=" << merge_1 / count << "ns "
    << "sort=" << sort / count << "us "
    << "init_buffer=" << init_buffer / count << "us "
    << endl;
}