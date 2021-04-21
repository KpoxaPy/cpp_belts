#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <queue>
#include <stdexcept>
#include <set>
using namespace std;

template <class T>
class ObjectPool {
public:
  T* Allocate() {
    T* o = TryAllocate();
    if (o == nullptr) {
      return MakeNew();
    }
    return o;
  }

  T* TryAllocate() {
    if (free_.empty()) {
      return nullptr;
    }
    T* o = free_.front();
    free_.pop();
    allocated_.insert(o);
    return o;
  }

  void Deallocate(T* object) {
    auto it = allocated_.find(object);
    if (it == allocated_.end()) {
      throw invalid_argument("");
    }
    allocated_.erase(it);
    free_.push(object);
  }

  ~ObjectPool() {
    for (T* p : allocated_) {
      delete p;
    }
    while (!free_.empty()) {
      delete free_.front();
      free_.pop();
    }
  }

private:
  set<T*> allocated_;
  queue<T*> free_;

  T* MakeNew() {
    T* o = new T;
    allocated_.insert(o);
    return o;
  }
};

void TestObjectPool() {
  ObjectPool<string> pool;

  auto p1 = pool.Allocate();
  auto p2 = pool.Allocate();
  auto p3 = pool.Allocate();

  *p1 = "first";
  *p2 = "second";
  *p3 = "third";

  pool.Deallocate(p2);
  ASSERT_EQUAL(*pool.Allocate(), "second");

  pool.Deallocate(p3);
  pool.Deallocate(p1);
  ASSERT_EQUAL(*pool.Allocate(), "third");
  ASSERT_EQUAL(*pool.Allocate(), "first");

  pool.Deallocate(p1);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestObjectPool);
  return 0;
}
