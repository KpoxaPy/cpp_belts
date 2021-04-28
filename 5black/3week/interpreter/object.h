#pragma once

#include "object_holder.h"
#include "runtime_error.h"

#include <ostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <unordered_map>

namespace Ast {
  class Statement;
}

class TestRunner;

namespace Runtime {

class Object {
public:
  virtual ~Object() = default;
  virtual void Print(std::ostream& os) = 0;
  virtual bool IsTrue() const {
    throw RuntimeError("Cannot cast Object to bool");
  }
};

template <typename T>
class ValueObject : public Object {
public:
  ValueObject(T v) : value(v) {
  }

  void Print(std::ostream& os) override {
    os << value;
  }

  const T& GetValue() const {
    return value;
  }

private:
  T value;
};

class String : public ValueObject<std::string> {
public:
  using ValueObject<std::string>::ValueObject;
  bool IsTrue() const override;
};

class Number : public ValueObject<int> {
public:
  using ValueObject<int>::ValueObject;
  bool IsTrue() const override;
};

class Bool : public ValueObject<bool> {
public:
  using ValueObject<bool>::ValueObject;
  void Print(std::ostream& os) override;
  bool IsTrue() const override;
};

struct Method {
  std::string name;
  std::vector<std::string> formal_params;
  std::unique_ptr<Ast::Statement> body;
};

class Class : public Object {
public:
  explicit Class(std::string name, std::vector<Method> methods, const Class* parent);
  const Method* GetMethod(const std::string& name) const;
  const std::string& GetName() const;
  void Print(std::ostream& os) override;
  bool IsTrue() const override;

private:
  std::string name_;
  std::unordered_map<std::string, Method> methods_;
  const Class* parent_;
};

class ClassInstance : public Object {
public:
  explicit ClassInstance(const Class& cls);

  void Print(std::ostream& os) override;
  bool IsTrue() const override;

  ObjectHolder Call(const std::string& method, const std::vector<ObjectHolder>& actual_args);
  bool HasMethod(const std::string& method, size_t argument_count) const;

  Closure& Fields();
  const Closure& Fields() const;

private:
  const Class& class_;
  Closure fields_;
};

void RunObjectsTests(TestRunner& test_runner);

}
