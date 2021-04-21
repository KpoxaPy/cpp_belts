#include "test_runner.h"
#include "profile.h"

#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <future>

using namespace std;

struct Stats {
  map<string, int> word_frequences;

  void operator+=(const Stats& other) {
    for (const auto& [k, v] : other.word_frequences) {
      word_frequences[k] += v;
    }
  }
};

template <typename InputIt>
Stats ExploreTokens(const set<string>& key_words, InputIt first, InputIt last) {
  Stats s;
  for (; first != last; ++first) {
    if (key_words.count(*first) > 0) {
      s.word_frequences[*first] += 1;
    }
  }
  return s;
}

Stats ExploreKeyWords(const set<string>& key_words, istream& input) {
  const size_t chunk_size = 50000;
  const size_t threads = 4;
  Stats s;
  vector<string> tokens(chunk_size * threads);
  while (input) {
    size_t i = 0;
    for (; i < tokens.size() && input >> tokens[i]; ++i) {
    }
    vector<future<Stats>> futs;
    for (size_t j = 0, k = min(i, chunk_size); j < i; j = k, k += min(i - j, chunk_size)) {
      futs.push_back(async([&key_words, &tokens, j, k]{
        return ExploreTokens(key_words, begin(tokens) + j, begin(tokens) + k);
      }));
    }
    for (auto& f : futs) {
      s += f.get();
    }
  }
  return s;
}

void TestBasic() {
  const set<string> key_words = {"yangle", "rocks", "sucks", "all"};

  stringstream ss;
  ss << "this new yangle service really rocks\n";
  ss << "It sucks when yangle isn't available\n";
  ss << "10 reasons why yangle is the best IT company\n";
  ss << "yangle rocks others suck\n";
  ss << "Goondex really sucks, but yangle rocks. Use yangle\n";

  const auto stats = ExploreKeyWords(key_words, ss);
  const map<string, int> expected = {
    {"yangle", 6},
    {"rocks", 2},
    {"sucks", 1}
  };
  ASSERT_EQUAL(stats.word_frequences, expected);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestBasic);
}
