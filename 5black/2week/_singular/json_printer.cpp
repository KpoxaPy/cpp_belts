#include "test_runner.h"

#include <cassert>
#include <cmath>
#include <functional>
#include <iomanip>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <type_traits>

using namespace std;

void PrintJsonString(std::ostream& out, std::string_view str) {
  out << quoted(str);
}

class EmptyContextType {};
static EmptyContextType EmptyContext{};

template <typename Parent = EmptyContextType>
class ArrayContextImpl;

template <typename Parent = EmptyContextType>
class ObjectContextImpl;

template <typename Parent = EmptyContextType>
class Context {
  using This = Context<Parent>;

 public:
  Context(ostream& os, Parent& parent = EmptyContext)
    : os_(os)
    , parent_(parent)
  {}

  ~Context() {
    if (!was_value_) {
      Null();
    }
  }

  Parent& String(string_view str) {
    before();
    PrintJsonString(os_, str);
    return parent_;
  }

  Parent& Number(int64_t v) {
    before();
    os_ << v;
    return parent_;
  }

  Parent& Null() {
    before();
    os_ << "null";
    return parent_;
  }
  
  Parent& Boolean(bool v) {
    before();
    os_ << (v ? "true" : "false");
    return parent_;
  }

  ArrayContextImpl<Parent> BeginArray() {
    before();
    return {os_, parent_};
  }

  ObjectContextImpl<Parent> BeginObject() {
    before();
    return {os_, parent_};
  }

 protected:
  ostream& os_;
  Parent& parent_;

  bool was_value_ = false;

  void before() {
    was_value_ = true;
    if constexpr (!is_same_v<Parent, EmptyContextType>) {
      parent_.before();
    }
  }
};

template <typename Parent>
class ArrayContextImpl : public Context<ArrayContextImpl<Parent>> {
  using Up = Context<ArrayContextImpl<Parent>>;

 public:
  ArrayContextImpl(ostream& os, Parent& parent = EmptyContext)
    : Up(os, *this)
    , parent_(parent)
  {
    Up::os_ << "[";
    Up::was_value_ = true;
  }

  ~ArrayContextImpl() {
    if (!was_end) {
      Up::os_ << "]";
    }
  }

  Parent& EndArray() {
    if (!was_end) {
      Up::os_ << "]";
      was_end = true;
    }
    return parent_;
  }

  void before() {
    if (!was_first) {
      was_first = true;
    } else {
      Up::os_ << ",";
    }
  }

 private:
  bool was_first = false;
  bool was_end = false;
  Parent& parent_;
};

template <typename Parent>
class KeyContext : public Context<Parent> {
  using Up = Context<Parent>;

 public:
  KeyContext(ostream& os, Parent& parent, string_view key)
    : Up(os, parent)
  {
    PrintJsonString(Up::os_, key);
    Up::os_ << ":";
  }
};

template <typename Parent>
class ObjectContextImpl {
  using This = ObjectContextImpl<Parent>;

 public:
  ObjectContextImpl(ostream& os, Parent& parent = EmptyContext)
    : os_(os)
    , parent_(parent)
  {
    os_ << "{";
  }

  ~ObjectContextImpl() {
    if (!was_end) {
      os_ << "}";
    }
  }

  Parent& EndObject() {
    if (!was_end) {
      os_ << "}";
      was_end = true;
    }
    return parent_;
  }

  KeyContext<This> Key(string_view str) {
    if (!was_first) {
      was_first = true;
    } else {
      os_ << ",";
    }
    return {os_, *this, str};
  }

  void before() {
  }

 private:
  ostream& os_;
  Parent& parent_;

  bool was_first = false;
  bool was_end = false;
};

using ArrayContext = ArrayContextImpl<EmptyContextType>;
ArrayContext PrintJsonArray(std::ostream& out) {
  return {out};
}

using ObjectContext = ObjectContextImpl<EmptyContextType>;
ObjectContext PrintJsonObject(std::ostream& out) {
  return {out};
}

void TestArray() {
  std::ostringstream output;

  {
    auto json = PrintJsonArray(output);
    json
      .Number(5)
      .Number(6)
      .BeginArray()
        .Number(7)
      .EndArray()
      .Number(8)
      .String("bingo!");
  }

  ASSERT_EQUAL(output.str(), R"([5,6,[7],8,"bingo!"])");
}

void TestEmptyArray() {
  std::ostringstream output;

  {
    PrintJsonArray(output);
  }

  ASSERT_EQUAL(output.str(), R"([])");
}

void TestObject() {
  std::ostringstream output;

  {
    auto json = PrintJsonObject(output);
    json
      .Key("id1").Number(1234)
      .Key("id2").Boolean(false)
      .Key("").Null()
      .Key("\"").String("\\");
  }

  ASSERT_EQUAL(output.str(), R"({"id1":1234,"id2":false,"":null,"\"":"\\"})");
}

void TestAutoClose() {
  std::ostringstream output;

  {
    auto json = PrintJsonArray(output);
    json.BeginArray().BeginObject();
  }

  ASSERT_EQUAL(output.str(), R"([[{}]])");
}

void Test1() {
  std::ostringstream output;

  {
    auto json = PrintJsonArray(output);
    json
      .String("hello")
      .Null()
      .BeginArray()
        .Number(10)
        .BeginObject()
        .EndObject()
        .String("")
        .Null()
        .BeginArray();
  }

  ASSERT_EQUAL(output.str(), R"(["hello",null,[10,{},"",null,[]]])");
}

void Test2() {
  std::ostringstream output;

  {
    auto json = PrintJsonObject(output);
    json
      .Key("foo")
      .BeginArray()
        .String("Hello")
      .EndArray()
      .Key("foo")
      .BeginObject()
        .Key("foo");
  }

  ASSERT_EQUAL(output.str(), R"({"foo":["Hello"],"foo":{"foo":null}})");
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestArray);
  RUN_TEST(tr, TestEmptyArray);
  RUN_TEST(tr, TestObject);
  RUN_TEST(tr, TestAutoClose);
  RUN_TEST(tr, Test1);
  RUN_TEST(tr, Test2);

  return 0;
}
