#include "index.h"
#include "string_util.h"
#include "profile.h"

#include <iostream>
#include <numeric>
#include <algorithm>
#include <map>

using namespace std;

namespace {
    const WordIndex EMPTY;
}

tuple<uint64_t, uint64_t> InvertedIndex::Add(string&& document) {
  DurationMeter<microseconds> m;

  docs.emplace_back(move(document));
  const size_t docid = docs.size() - 1;

  m.Start();
  auto v = SplitIntoWords(docs.back());
  uint64_t split_time = m.Get();
  m.Start();
  for (const string_view& word : v) {
      WordIndex& idx = index[word];
      if (!idx.empty() && idx.back().first == docid) {
          ++idx.back().second;
      } else {
          idx.emplace_back(docid, 1);
      }
  }
  uint64_t push_time = m.Get();
  return {split_time, push_time};
}

const WordIndex& InvertedIndex::Lookup(string_view word) const {
  if (auto it = index.find(word); it != index.end()) {
    return it->second;
  } else {
    return EMPTY;
  }
}

const string& InvertedIndex::GetDocument(size_t id) const {
  return docs[id];
}

void InvertedIndex::Search(std::string_view query, SearchTimers& ts, WordIndex& result) const {
    DurationMeter<nanoseconds> m;

    m.Start();
    InitIndexBuffer(result);
    get<2>(ts) += m.Get() / 1000;
    
    for (const string_view& word : SplitIntoWords(query)) {
        m.Start();
        const WordIndex& w_idx = Lookup(word);
        get<0>(ts) += m.Get() / 1000;

        m.Start();
        for (const auto& p : w_idx) {
            result[p.first].second += p.second;
        }
        auto t = m.Get();
        get<1>(ts) += t / 1000;
        if (w_idx.size() > 0) {
            get<3>(ts) += t / w_idx.size();
        }
    }
}

void InvertedIndex::InitIndexBuffer(WordIndex& buf) const {
    if (buf.capacity() != docs.size()) {
        buf.reserve(docs.size());
        buf.resize(docs.size());
    }
    for (size_t i = 0; i < buf.size(); ++i) {
        buf[i] = {i, 0};
    }
}
