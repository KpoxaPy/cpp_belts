#include "search_result.h"
#include "iterator_range.h"
#include "profile.h"

#include <algorithm>
#include <iterator>

using namespace std;

SearchResult::SearchResult(std::string query, WordIndex idx, uint64_t& t)
    : query(move(query))
    , results(move(idx))
{
    DurationMeter<microseconds> m; 
    m.Start();
    sort();
    t += m.Get();
}

void SearchResult::sort() {
    partial_sort(
        results.begin(),
        min(results.begin() + 5, results.end()),
        results.end(),
        [](pair<size_t, size_t> lhs, pair<size_t, size_t> rhs) {
            int64_t lhs_docid = lhs.first;
            auto lhs_hit_count = lhs.second;
            int64_t rhs_docid = rhs.first;
            auto rhs_hit_count = rhs.second;
            return make_pair(lhs_hit_count, -lhs_docid) > make_pair(rhs_hit_count, -rhs_docid);
        });
}

ostream& operator<<(ostream& s, const SearchResult& r) {
    s << r.query << ':';
    for (auto [docid, hitcount] : Head(r.results, 5)) {
      if (hitcount == 0) {
          break;
      }

      s << " {"
        << "docid: " << docid << ", "
        << "hitcount: " << hitcount << '}';
    }
    s << '\n';
    return s;
}