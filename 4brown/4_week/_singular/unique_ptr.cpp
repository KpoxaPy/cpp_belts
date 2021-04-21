#include "test_runner.h"

#include <cstddef>  // нужно для nullptr_t

using namespace std;

template <typename T>
class UniquePtr {
private:
  T* ptr = nullptr;

public:
  UniquePtr() {}
  UniquePtr(T * ptr) : ptr(ptr) {}

  UniquePtr(const UniquePtr&) = delete;
  UniquePtr& operator = (const UniquePtr&) = delete;

  UniquePtr(UniquePtr&& other) {
    *this = move(other);
  }

  UniquePtr& operator = (UniquePtr&& other) {
    if (this != &other) {
      UniquePtr p;
      p.Swap(other);
      Swap(p);
    }
    return *this;
  }

  UniquePtr& operator = (nullptr_t) {
    UniquePtr tmp;
    Swap(tmp);
    return *this;
  }

  ~UniquePtr() {
    if (ptr != nullptr) {
      delete ptr;
    }
  }

  T& operator * () const {
    return *ptr;
  }

  T * operator -> () const {
    return ptr;
  }

  T * Release() {
    auto p = ptr;
    ptr = nullptr;
    return p;
  }

  void Reset(T * ptr) {
    UniquePtr null(ptr);
    Swap(null);
  }

  void Swap(UniquePtr& other) {
    if (this != &other) {
      swap(ptr, other.ptr);
    }
  }

  T * Get() const {
    return ptr;
  }
};


struct Item {
  static int counter;
  int value;
  Item(int v = 0): value(v) {
    ++counter;
  }
  Item(const Item& other): value(other.value) {
    ++counter;
  }
  ~Item() {
    --counter;
  }
};

int Item::counter = 0;


void TestLifetime() {
  Item::counter = 0;
  {
    UniquePtr<Item> ptr(new Item);
    ASSERT_EQUAL(Item::counter, 1);

    ptr.Reset(new Item);
    ASSERT_EQUAL(Item::counter, 1);
  }
  ASSERT_EQUAL(Item::counter, 0);

  {
    UniquePtr<Item> ptr(new Item);
    ASSERT_EQUAL(Item::counter, 1);

    auto rawPtr = ptr.Release();
    ASSERT_EQUAL(Item::counter, 1);

    delete rawPtr;
    ASSERT_EQUAL(Item::counter, 0);
  }
  ASSERT_EQUAL(Item::counter, 0);
}

void TestGetters() {
  UniquePtr<Item> ptr(new Item(42));
  ASSERT_EQUAL(ptr.Get()->value, 42);
  ASSERT_EQUAL((*ptr).value, 42);
  ASSERT_EQUAL(ptr->value, 42);
}

void TestMove() {
  UniquePtr<Item> p(new Item(42));
  UniquePtr<Item> p2(move(p));
  UniquePtr<Item> p3;
  p3 = move(p2);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestLifetime);
  RUN_TEST(tr, TestGetters);
  RUN_TEST(tr, TestMove);
}
