#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

template <class T>
ostream& operator << (ostream& os, const vector<T>& s) {
  os << "{";
  bool first = true;
  for (const auto& x : s) {
    if (!first) {
      os << ", ";
    }
    first = false;
    os << x;
  }
  return os << "}";
}

template <class T>
ostream& operator << (ostream& os, const set<T>& s) {
  os << "{";
  bool first = true;
  for (const auto& x : s) {
    if (!first) {
      os << ", ";
    }
    first = false;
    os << x;
  }
  return os << "}";
}

template <class K, class V>
ostream& operator << (ostream& os, const map<K, V>& m) {
  os << "{";
  bool first = true;
  for (const auto& kv : m) {
    if (!first) {
      os << ", ";
    }
    first = false;
    os << kv.first << ": " << kv.second;
  }
  return os << "}";
}

template<class T, class U>
void AssertEqual(const T& t, const U& u, const string& hint = {}) {
  if (t != u) {
    ostringstream os;
    os << "Assertion failed: " << t << " != " << u;
    if (!hint.empty()) {
      os << " hint: " << hint;
    }
    throw runtime_error(os.str());
  }
}

void Assert(bool b, const string& hint) {
  AssertEqual(b, true, hint);
}

class TestRunner {
public:
  template <class TestFunc>
  void RunTest(TestFunc func, const string& test_name) {
    try {
      func();
      cerr << test_name << " OK" << endl;
    } catch (exception& e) {
      ++fail_count;
      cerr << test_name << " fail: " << e.what() << endl;
    } catch (...) {
      ++fail_count;
      cerr << "Unknown exception caught" << endl;
    }
  }

  ~TestRunner() {
    if (fail_count > 0) {
      cerr << fail_count << " unit tests failed. Terminate" << endl;
      exit(1);
    }
  }

private:
  int fail_count = 0;
};

// Make expected answer
string MEA(tuple<string, string> args) {
    string fn, ln;
    tie(fn, ln) = args;
    if (fn.empty() && ln.empty()) {
        return "Incognito";
    } else if (fn.empty()) {
        return ln + " with unknown first name";
    } else if (ln.empty()) {
        return fn + " with unknown last name";
    } else {
        return fn + " " + ln;
    }
}

void TestIncognito() {
    Person p;
    AssertEqual(p.GetFullName(1900), MEA({}), "Empty returns incognito");
    p.ChangeFirstName(1901, "Fn1");
    AssertEqual(p.GetFullName(1900), MEA({}), "Past returns incognito");
    p.ChangeLastName(1902, "Ln1");
    AssertEqual(p.GetFullName(1900), MEA({}), "Past returns incognito");
}

void TestPart() {
    {
        Person p;
        p.ChangeFirstName(1901, "Fn1");
        p.ChangeLastName(1902, "Ln1");
        AssertEqual(p.GetFullName(1901), MEA({"Fn1", ""}), "Part first name");
        AssertEqual(p.GetFullName(1902), MEA({"Fn1", "Ln1"}), "Full name");
        AssertEqual(p.GetFullName(1903), MEA({"Fn1", "Ln1"}), "Full name after");
    }
    {
        Person p;
        p.ChangeLastName(1901, "Ln1");
        p.ChangeFirstName(1902, "Fn1");
        AssertEqual(p.GetFullName(1901), MEA({"", "Ln1"}), "Part last name");
        AssertEqual(p.GetFullName(1902), MEA({"Fn1", "Ln1"}), "Full name");
        AssertEqual(p.GetFullName(1903), MEA({"Fn1", "Ln1"}), "Full name after");
    }
}

void TestAddInPast() {
    {
        Person p;
        AssertEqual(p.GetFullName(1910), MEA({}), "Incognito");
        p.ChangeFirstName(1905, "Fn1");
        AssertEqual(p.GetFullName(1910), MEA({"Fn1", ""}), "Part first name");
        p.ChangeLastName(1902, "Ln1");
        AssertEqual(p.GetFullName(1904), MEA({"", "Ln1"}), "Part last name");
        AssertEqual(p.GetFullName(1910), MEA({"Fn1", "Ln1"}), "Full name");
    }
    {
        Person p;
        AssertEqual(p.GetFullName(1910), MEA({}), "Incognito");
        p.ChangeLastName(1905, "Ln1");
        AssertEqual(p.GetFullName(1910), MEA({"", "Ln1"}), "Part last name");
        p.ChangeFirstName(1902, "Fn1");
        AssertEqual(p.GetFullName(1904), MEA({"Fn1", ""}), "Part first name");
        AssertEqual(p.GetFullName(1910), MEA({"Fn1", "Ln1"}), "Full name");
    }
}

int main() {
  TestRunner runner;
  runner.RunTest(TestIncognito, "TestIncognito");
  runner.RunTest(TestPart, "TestPart");
  runner.RunTest(TestAddInPast, "TestAddInPast");
  return 0;
}
