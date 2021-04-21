#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

using namespace std;

template <typename It>
class Range {
public:
  Range(It begin, It end) : begin_(begin), end_(end) {}
  It begin() const { return begin_; }
  It end() const { return end_; }

private:
  It begin_;
  It end_;
};

pair<string_view, optional<string_view>> SplitTwoStrict(string_view s, string_view delimiter = " ") {
  const size_t pos = s.find(delimiter);
  if (pos == s.npos) {
    return {s, nullopt};
  } else {
    return {s.substr(0, pos), s.substr(pos + delimiter.length())};
  }
}

vector<string_view> Split(string_view s, string_view delimiter = " ") {
  vector<string_view> parts;
  if (s.empty()) {
    return parts;
  }
  while (true) {
    const auto [lhs, rhs_opt] = SplitTwoStrict(s, delimiter);
    parts.push_back(lhs);
    if (!rhs_opt) {
      break;
    }
    s = *rhs_opt;
  }
  return parts;
}


class Domain {
public:
  explicit Domain(string_view text) {
    vector<string_view> parts = Split(text, ".");
    parts_reversed_.assign(rbegin(parts), rend(parts));
  }

  size_t GetPartCount() const {
    return parts_reversed_.size();
  }

  auto GetParts() const {
    return Range(rbegin(parts_reversed_), rend(parts_reversed_));
  }
  auto GetReversedParts() const {
    return Range(begin(parts_reversed_), end(parts_reversed_));
  }

  bool operator==(const Domain& other) const {
    return parts_reversed_ == other.parts_reversed_;
  }

private:
  vector<string> parts_reversed_;
};

ostream& operator<<(ostream& stream, const Domain& domain) {
  bool first = true;
  for (const string_view part : domain.GetParts()) {
    if (!first) {
      stream << '.';
    } else {
      first = false;
    }
    stream << part;
  }
  return stream;
}

// domain is subdomain of itself
bool IsSubdomain(const Domain& subdomain, const Domain& domain) {
  const auto subdomain_reversed_parts = subdomain.GetReversedParts();
  const auto domain_reversed_parts = domain.GetReversedParts();
  return
      subdomain.GetPartCount() >= domain.GetPartCount()
      && equal(begin(domain_reversed_parts), end(domain_reversed_parts),
               begin(subdomain_reversed_parts));
}

bool IsSubOrSuperDomain(const Domain& lhs, const Domain& rhs) {
  return lhs.GetPartCount() >= rhs.GetPartCount()
         ? IsSubdomain(lhs, rhs)
         : IsSubdomain(rhs, lhs);
}


class DomainChecker {
public:
  template <typename InputIt>
  DomainChecker(InputIt domains_begin, InputIt domains_end) {
    sorted_domains_.reserve(distance(domains_begin, domains_end));
    for (const Domain& domain : Range(domains_begin, domains_end)) {
      sorted_domains_.push_back(&domain);
    }
    sort(begin(sorted_domains_), end(sorted_domains_), IsDomainLess);
    sorted_domains_ = AbsorbSubdomains(move(sorted_domains_));
  }

  // Check if candidate is subdomain of some domain
  bool IsSubdomain(const Domain& candidate) const {
    const auto it = upper_bound(
        begin(sorted_domains_), end(sorted_domains_),
        &candidate, IsDomainLess);
    if (it == begin(sorted_domains_)) {
      return false;
    }
    return ::IsSubdomain(candidate, **prev(it));
  }

private:
  vector<const Domain*> sorted_domains_;

  static bool IsDomainLess(const Domain* lhs, const Domain* rhs) {
    const auto lhs_reversed_parts = lhs->GetReversedParts();
    const auto rhs_reversed_parts = rhs->GetReversedParts();
    return lexicographical_compare(
      begin(lhs_reversed_parts), end(lhs_reversed_parts),
      begin(rhs_reversed_parts), end(rhs_reversed_parts)
    );
  }

  static vector<const Domain*> AbsorbSubdomains(vector<const Domain*> domains) {
    domains.erase(
        unique(begin(domains), end(domains),
               [](const Domain* lhs, const Domain* rhs) {
                 return IsSubOrSuperDomain(*lhs, *rhs);
               }),
        end(domains)
    );
    return domains;
  }
};


vector<Domain> ReadDomains(istream& in_stream = cin) {
  vector<Domain> domains;

  size_t count;
  in_stream >> count;
  domains.reserve(count);

  for (size_t i = 0; i < count; ++i) {
    string domain_text;
    in_stream >> domain_text;
    domains.emplace_back(domain_text);
  }
  return domains;
}

vector<bool> CheckDomains(const vector<Domain>& banned_domains, const vector<Domain>& domains_to_check) {
  const DomainChecker checker(begin(banned_domains), end(banned_domains));

  vector<bool> check_results;
  check_results.reserve(domains_to_check.size());
  for (const Domain& domain_to_check : domains_to_check) {
    check_results.push_back(!checker.IsSubdomain(domain_to_check));
  }

  return check_results;
}

void PrintCheckResults(const vector<bool>& check_results, ostream& out_stream = cout) {
  for (const bool check_result : check_results) {
    out_stream << (check_result ? "Good" : "Bad") << "\n";
  }
}

void TestPart(string_view test, string_view delimiter, const vector<string>& expected) {
  vector<string> result;
  for (auto s : Split(test, delimiter)) {
    result.push_back(string(s));
  }
  ASSERT_EQUAL(result, expected);
}

void TestParts() {
  TestPart("", " ", {});
  TestPart("abc", " ", {"abc"});
  TestPart("a b", " ", {"a", "b"});
  TestPart("a.b", ".", {"a", "b"});
  TestPart("a...b", "...", {"a", "b"});
  TestPart("a...b", ".", {"a", "", "", "b"});
}

void TestDomainPart(string_view domain, const vector<string>& expected) {
  Domain d{domain};
  ASSERT_EQUAL(d.GetPartCount(), expected.size());

  auto parts = d.GetParts();
  vector<string> straight;
  copy(parts.begin(), parts.end(), back_inserter(straight));
  ASSERT_EQUAL(straight, expected);

  auto reversed_parts = d.GetReversedParts();
  vector<string> reversed;
  copy(reversed_parts.begin(), reversed_parts.end(), back_inserter(reversed)); 
  vector<string> reversed_expected;
  copy(expected.rbegin(), expected.rend(), back_inserter(reversed_expected));
  ASSERT_EQUAL(reversed, reversed_expected);
}

void TestDomain() {
  TestDomainPart("", {});
  TestDomainPart("a", {"a"});
  TestDomainPart("a.b", {"a", "b"});
  TestDomainPart("a.b.c", {"a", "b", "c"});
}

bool TestSubDomainPart(const string& subdomain, const string& domain) {
  return IsSubdomain(Domain{subdomain}, Domain{domain});
}

void TestSubDomain() {
  ASSERT(TestSubDomainPart("", ""));
  ASSERT(TestSubDomainPart("a", ""));
  ASSERT(TestSubDomainPart("a", "a"));
  ASSERT(TestSubDomainPart("b.a", "a"));
  ASSERT(!TestSubDomainPart("a", "b.a"));
  ASSERT(!TestSubDomainPart("a.b", "a"));
}

void TestAbsorbSubdomains() {
  vector<Domain> banned_domains = {
    Domain{"a"},
    Domain{"b.a"},
    Domain{"c.b.a"},
    Domain{"b"},
    Domain{"c.b"},
    Domain{"c"}
  };
  DomainChecker ch(banned_domains.begin(), banned_domains.end());
  ASSERT(ch.IsSubdomain(Domain{"d.a"}));
  ASSERT(ch.IsSubdomain(Domain{"d.b.a"}));
  ASSERT(ch.IsSubdomain(Domain{"d.c.b.a"}));
  ASSERT(ch.IsSubdomain(Domain{"d.c.b.a"}));
  ASSERT(ch.IsSubdomain(Domain{"d.b"}));
  ASSERT(ch.IsSubdomain(Domain{"d.c.b"}));
  ASSERT(ch.IsSubdomain(Domain{"d.c"}));
}

void TestCheckDomains() {
  vector<Domain> banned;
  banned.emplace_back("a");

  vector<Domain> check;
  check.emplace_back("b.a");
  check.emplace_back("a.b");

  vector<bool> expected = {
    false,
    true
  };
  ASSERT_EQUAL(CheckDomains(banned, check), expected);
}

void TestCheckResults() {
  ostringstream s;
  PrintCheckResults(vector<bool>{true, false}, s);
  ASSERT_EQUAL(s.str(), "Good\nBad\n");
}

void TestReadDomains() {
  istringstream s{
    "4\n"
    "a\n"
    "b\n"
    "c\n"
    "d\n"
  };
  vector<Domain> expected;
  expected.emplace_back("a");
  expected.emplace_back("b");
  expected.emplace_back("c");
  expected.emplace_back("d");
  ASSERT_EQUAL(ReadDomains(s), expected);
}

void TestAll() {
  TestRunner tr;
  RUN_TEST(tr, TestParts);
  RUN_TEST(tr, TestDomain);
  RUN_TEST(tr, TestSubDomain);
  RUN_TEST(tr, TestAbsorbSubdomains);
  RUN_TEST(tr, TestCheckDomains);
  RUN_TEST(tr, TestCheckResults);
  RUN_TEST(tr, TestReadDomains);
}

int main() {
  TestAll();

  const vector<Domain> banned_domains = ReadDomains();
  const vector<Domain> domains_to_check = ReadDomains();
  PrintCheckResults(CheckDomains(banned_domains, domains_to_check));
  return 0;
}
