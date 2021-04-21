#include "test_runner.h"

#include <functional>
#include <string>
#include <optional>
#include <shared_mutex>
#include <future>
#include <thread>

using namespace std;

template <typename T>
class LazyValue {
public:
  explicit LazyValue(std::function<T()> init)
    : init_(init)
  {}

  bool HasValue() const {
    return value_.has_value();
  }

  const T& Get() const {
    bool should_try_initialize = false;
    if (shared_lock l(m_); !value_) {
      should_try_initialize = true;
    }
    if (should_try_initialize) {
      if (lock_guard g(m_); !value_) {
        value_.emplace(init_());
      }
    }
    return value_.value();
  }

private:
  std::function<T()> init_;
  mutable std::optional<T> value_;
  mutable std::shared_mutex m_;
};

void UseExample() {
  const string big_string = "Giant amounts of memory";

  LazyValue<string> lazy_string([&big_string] { return big_string; });

  ASSERT(!lazy_string.HasValue());
  ASSERT_EQUAL(lazy_string.Get(), big_string);
  ASSERT_EQUAL(lazy_string.Get(), big_string);
}

void TestInitializerIsntCalled() {
  bool called = false;

  {
    LazyValue<int> lazy_int([&called] {
      called = true;
      return 0;
    });
  }
  ASSERT(!called);
}

class NotDefaultConstructedString : public string {
public:
  using string::string;

  NotDefaultConstructedString() = delete;
};

void TestNotDefaultConstructed() {
  LazyValue<NotDefaultConstructedString> test([]{ return ""; });
}

void TestOnlyOneInit() {
  {
    mutex m;
    size_t called = 0;
    const string big_string = "Giant amounts of memory";

    LazyValue<string> lazy([&big_string, &called, &m] () mutable {
      this_thread::sleep_for(500ms);
      lock_guard g(m);
      ++called;
      return big_string;
    });

    {
      vector<future<void>> futs;
      for (int i = 0; i < 5; ++i) {
        futs.push_back(async([&lazy, &big_string] {
          ASSERT_EQUAL(lazy.Get(), big_string);
        }));
      }
    }
    ASSERT_EQUAL(called, static_cast<size_t>(1));

    called = 0;
    {
      vector<future<void>> futs;
      for (int i = 0; i < 50; ++i) {
        futs.push_back(async([&lazy, &big_string] {
          ASSERT_EQUAL(lazy.Get(), big_string);
        }));
      }
    }
    ASSERT_EQUAL(called, static_cast<size_t>(0));
  }
}

void TestAll() {
  TestRunner tr;
  RUN_TEST(tr, UseExample);
  RUN_TEST(tr, TestInitializerIsntCalled);
  RUN_TEST(tr, TestNotDefaultConstructed);
  RUN_TEST(tr, TestOnlyOneInit);
}

int main() {
  TestAll();
  return 0;
}
