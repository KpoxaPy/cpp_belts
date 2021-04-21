#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "test_runner.h"

#define DEBUG false

using namespace std;

template <typename Functor>
void ReadDomainsAndDo(istream& s, Functor f) {
  size_t count;
  s >> count;

  string domain;
  for (size_t i = 0; i < count; ++i) {
    s >> domain;
    f(domain);
  }
}

class DomainFilter {
 public:
  DomainFilter(istream& s) {
    ReadDomainsAndDo(s, [this](const string& domain) mutable {
      banned_.emplace_back(PrepareDomain(domain));
    });
    sort(banned_.begin(), banned_.end());
    banned_.erase(unique(banned_.begin(), banned_.end(), [](string_view lhs, string_view rhs) {
      return lhs == rhs.substr(0, lhs.size());
    }), banned_.end());
  }

  bool IsBad(const string& domain) const;

 private:
  vector<string> banned_;

  static string PrepareDomain(string_view s) {
    string res;
    res.reserve(s.size() + 1);
    reverse_copy(s.begin(), s.end(), back_inserter(res));
    res.push_back('.');
    return res;
  }
};


bool DomainFilter::IsBad(const string& domain) const {
  const string tmp = PrepareDomain(domain);
  const string_view tmp_view = tmp;
  auto it = lower_bound(banned_.begin(), banned_.end(), tmp_view,
    [](const string& elem, const string_view& value){
      if (DEBUG) {
        cout << "Search for \"" << value << "\"> comparing: \"" << elem << "\" < \"" << value.substr(0, elem.size()) << "\"" << endl;
      }
      return elem < value.substr(0, elem.size());
    }
  );
  return it != banned_.end() && *it == tmp_view.substr(0, it->size()); 
}

void TestDomains(istream& in, ostream& out, const DomainFilter& filter) {
  ReadDomainsAndDo(in, [&filter, &out](const string& domain) mutable {
    out << (filter.IsBad(domain) ? "Bad" : "Good") << endl;
  });
}

void TestFullDomainMatch() {
  istringstream filter_in(
    "1 example.com\n"
  );
  DomainFilter f(filter_in);

  istringstream test_in(
    "4 com example.com test-example.com test.example.com\n"
  );
  ostringstream out;
  TestDomains(test_in, out, f);
  ASSERT_EQUAL(out.str(), "Good\nBad\nGood\nBad\n");
}

void TestSubdomainOverbanned() {
  istringstream filter_in(
    "2 b.c a.b.c\n"
  );
  DomainFilter f(filter_in);

  istringstream test_in(
    "1 d.b.c\n"
  );
  ostringstream out;
  TestDomains(test_in, out, f);
  ASSERT_EQUAL(out.str(), "Bad\n");
}

void TestAll() {
  TestRunner tr;
  RUN_TEST(tr, TestFullDomainMatch);
  RUN_TEST(tr, TestSubdomainOverbanned);
  // RUN_TEST(tr, Test);
}

int main() {
  TestAll();

  DomainFilter filter(cin);
  TestDomains(cin, cout, filter);

  return 0;
}
