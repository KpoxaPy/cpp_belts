#include "test_runner.h"
#include <string>
#include <string_view>
#include <map>

using namespace std;

class Translator {
public:
  void Add(string_view source, string_view target) {
    auto r = fwd_.emplace(source, target);
    if (!r.second) {
      bwd_.erase(r.first->second);
      r.first->second = target;
    }
    bwd_.insert_or_assign(r.first->second, r.first->first);
  }

  string_view TranslateForward(string_view source) const {
    if (auto it = fwd_.find(string(source)); it != fwd_.end()) {
      return it->second;
    }
    return EMPTY;
  }

  string_view TranslateBackward(string_view target) const {
    if (auto it = bwd_.find(string(target)); it != bwd_.end()) {
      return it->second;
    }
    return EMPTY;
  }

  void Print() const {
    cout << "Fwd:" << endl;
    for (auto [s, t] : fwd_) {
      cout << s << " -> " << t << endl;
    }
    cout << "Bwd:" << endl;
    for (auto [s, t] : bwd_) {
      cout << s << " -> " << t << endl;
    }
  }

private:
  static const string EMPTY;

  map<string, string> fwd_;
  map<string_view, string_view> bwd_;
};

const string Translator::EMPTY = "";

void TestSimple() {
  Translator translator;
  translator.Add(string("okno"), string("window"));
  translator.Add(string("stol"), string("table"));

  ASSERT_EQUAL(translator.TranslateForward("okno"), "window");
  ASSERT_EQUAL(translator.TranslateBackward("table"), "stol");
  ASSERT_EQUAL(translator.TranslateBackward("stol"), "");
}

void Test2() {
  Translator translator;
  translator.Add(string("1"), string("1"));
  translator.Add(string("1"), string("2"));
  translator.Print();

  ASSERT_EQUAL(translator.TranslateForward("1"), "2");
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestSimple);
  RUN_TEST(tr, Test2);
  return 0;
}
