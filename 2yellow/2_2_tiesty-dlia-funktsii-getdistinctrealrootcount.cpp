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
  os << "[";
  bool first = true;
  for (const auto& x : s) {
    if (!first) {
      os << ", ";
    }
    first = false;
    os << x;
  }
  return os << "]";
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

int GetDistinctRealRootCount(double a, double b, double c) {
    return -1;
}

int W(vector<double> args) {
    return GetDistinctRealRootCount(args[0], args[1], args[2]);
}

void TestConst() {
    AssertEqual(W({0, 0, 1}), 0, "C = 0, C != 0 => zero roots");
}

void TestLinear() {
    AssertEqual(W({0, 1, 0}), 1, "x = 0, one root");
    AssertEqual(W({0, 1, 1}), 1, "x + 1 = 0, one root");
}

void TestQ() {
    AssertEqual(W({1, 0, 0}), 1, "x^2 = 0, one root");
    AssertEqual(W({1, 0, 1}), 0, "x^2 + 1 = 0, zero roots");
    AssertEqual(W({1, 0, -1}), 2, "x^2 - 1 = 0, two roots");
}

int main() {
  TestRunner runner;
  runner.RunTest(TestConst, "TestConst");
  runner.RunTest(TestLinear, "TestLinear");
  runner.RunTest(TestQ, "TestQ");
  return 0;
}
