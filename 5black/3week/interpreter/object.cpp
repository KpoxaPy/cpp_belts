#include "object.h"
#include "statement.h"

#include <sstream>
#include <string_view>
#include <iterator>

using namespace std;

namespace Runtime {

bool String::IsTrue() const {
  return GetValue().length() > 0;
}

ObjectHolder String::Add(ObjectHolder rhs) const {
  if (auto obj = rhs.TryAs<String>(); obj) {
    return ObjectHolder::Own(String(GetValue() + obj->GetValue()));
  }
  throw RuntimeError("Strings can be added to strings only");
}

bool Number::IsTrue() const {
  return GetValue() == 0;
}

ObjectHolder Number::Add(ObjectHolder rhs) const {
  if (auto obj = rhs.TryAs<Number>(); obj) {
    return ObjectHolder::Own(Number(GetValue() + obj->GetValue()));
  }
  throw RuntimeError("Numbers can be added to numbers only");
}

ObjectHolder Number::Sub(ObjectHolder rhs) const {
  if (auto obj = rhs.TryAs<Number>(); obj) {
    return ObjectHolder::Own(Number(GetValue() - obj->GetValue()));
  }
  throw RuntimeError("Numbers can be substituted with numbers only");
}

ObjectHolder Number::Mult(ObjectHolder rhs) const {
  if (auto obj = rhs.TryAs<Number>(); obj) {
    return ObjectHolder::Own(Number(GetValue() * obj->GetValue()));
  }
  throw RuntimeError("Numbers can be multiplied by numbers only");
}

ObjectHolder Number::Div(ObjectHolder rhs) const {
  if (auto obj = rhs.TryAs<Number>(); obj) {
    auto right = obj->GetValue();
    if (right == 0) {
      throw RuntimeError("Division by zero!");
    }
    return ObjectHolder::Own(Number(GetValue() / right));
  }
  throw RuntimeError("Numbers can be divided by numbers only");
}

void Bool::Print(std::ostream& os) {
  if (GetValue()) {
    os << "True";
  } else {
    os << "False";
  }
}

bool Bool::IsTrue() const {
  return GetValue();
}

Class::Class(std::string name, std::vector<Method> methods, const Class* parent)
  : name_(move(name))
  , parent_(parent)
{
  for (auto it = make_move_iterator(methods.begin())
    ; it != make_move_iterator(methods.end())
    ; ++it)
  {
    methods_.emplace(it->name, *it);
  }
}

const Method* Class::GetMethod(const std::string& name) const {
  if (auto it = methods_.find(name); it != methods_.end()) {
    return &it->second;
  }
  return nullptr;
}

void Class::Print(ostream& /* os */) {
  throw RuntimeError("Class::Print is not implemented yet"); // FIXME
}

bool Class::IsTrue() const {
  return true;
}

const std::string& Class::GetName() const {
  return name_;
}

void ClassInstance::Print(std::ostream& /* os */) {
  throw RuntimeError("ClassInstance::Print is not implemented yet"); // FIXME
}

bool ClassInstance::IsTrue() const {
  return true;
}

bool ClassInstance::HasMethod(const std::string& /* method */, size_t /* argument_count */) const {
  throw RuntimeError("ClassInstance::HasMethod is not implemented yet"); // FIXME
}

const Closure& ClassInstance::Fields() const {
  return fields_;
}

Closure& ClassInstance::Fields() {
  return fields_;
}

ClassInstance::ClassInstance(const Class& cls)
  : class_(cls)
{
}

ObjectHolder ClassInstance::Call(const std::string& /* method */, const std::vector<ObjectHolder>& /* actual_args */) {
  throw RuntimeError("ClassInstance::Call is not implemented yet"); // FIXME
}

} /* namespace Runtime */
