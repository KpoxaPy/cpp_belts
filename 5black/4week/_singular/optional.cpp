#include "optional.h"

#include "test_runner.h"

using namespace std;

class C {
public:
  inline static int created = 0;
  inline static int assigned = 0;
  inline static int deleted = 0;
  inline static int moved = 0;
  static void Reset() {
    created = assigned = deleted = moved = 0;
  }

  C() {
    ++created;
  }
  C(const C&) {
    ++created;
  }
  C(C&&) {
    ++moved;
  }
  C& operator=(const C&) {
    ++assigned;
    return *this;
  }
  C& operator=(C&&) {
    ++moved;
    return *this;
  }
  ~C() {
    ++deleted;
  }
};

void TestInit() {
  {
    C::Reset();
    C c;
    Optional<C> o(c);
    ASSERT(C::created == 2 && C::assigned == 0 && C::deleted == 0 && C::moved == 0);
  }
  ASSERT(C::deleted == 2);
};


void TestAssign() {
  Optional<C> o1, o2;

  { // Assign a Value to empty
    C::Reset();
    C c;
    o1 = c;
    ASSERT(C::created == 2 && C::assigned == 0 && C::deleted == 0 && C::moved == 0);
  }
  { // Assign a non-empty to empty
    C::Reset();
    o2 = o1;
    ASSERT(C::created == 1 && C::assigned == 0 && C::deleted == 0 && C::moved == 0);
  }
  { // Assign non-empty to non-empty
    C::Reset();
    o2 = o1;
    ASSERT(C::created == 0 && C::assigned == 1 && C::deleted == 0 && C::moved == 0);
  }
}

void TestMove() {
  Optional<C> o1, o2;

  { // Move a Value to empty
    C c;
    C::Reset();
    o1 = std::move(c);
    ASSERT(C::created == 0 && C::assigned == 0 && C::deleted == 0 && C::moved == 1);
    ASSERT(o1.HasValue());
  }
  { // Move a Value to non-empty
    C c;
    C::Reset();
    o1 = std::move(c);
    ASSERT(C::created == 0 && C::assigned == 0 && C::deleted == 0 && C::moved == 1);
    ASSERT(o1.HasValue());
  }
  { // Move non-empty to empty
    C::Reset();
    o2 = std::move(o1);
    ASSERT(C::created == 0 && C::assigned == 0 && C::deleted == 0 && C::moved == 1);
    ASSERT(o2.HasValue());
  }
  { // Move non-empty to non-empty
    o1 = C();
    C::Reset();
    o2 = std::move(o1);
    ASSERT(C::created == 0 && C::assigned == 0 && C::deleted == 0 && C::moved == 1);
    ASSERT(o2.HasValue());
  }
  { // Move construction from value
    C c;
    C::Reset();
    Optional<C> o = std::move(c);
    ASSERT(C::created == 0 && C::assigned == 0 && C::deleted == 0 && C::moved == 1);
    ASSERT(o.HasValue());
  }
  { // Move construction from empty
    o1.Reset();
    C::Reset();
    Optional<C> o = std::move(o1);
    ASSERT(C::created == 0 && C::assigned == 0 && C::deleted == 0 && C::moved == 0);
    ASSERT(!o.HasValue());
  }
  { // Move construction from non-empty
    o1 = C();
    C::Reset();
    Optional<C> o = std::move(o1);
    ASSERT(C::created == 0 && C::assigned == 0 && C::deleted == 0 && C::moved == 1);
    ASSERT(o.HasValue());
  }
}

void TestReset() {
  C::Reset();
  Optional<C> o = C();
  o.Reset();
  ASSERT(C::created == 1 && C::assigned == 0 && C::deleted == 2 && C::moved == 1);
}

void TestHasValue() {
  Optional<int> o;
  ASSERT(!o.HasValue());

  o = 42;
  ASSERT(o.HasValue());

  o.Reset();
  ASSERT(!o.HasValue());
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestInit);
  RUN_TEST(tr, TestAssign);
  RUN_TEST(tr, TestMove);
  RUN_TEST(tr, TestReset);
  RUN_TEST(tr, TestHasValue);
  return 0;
}
