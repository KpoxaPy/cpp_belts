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

ObjectHolder String::Add(ObjectHolder rhs) {
  if (auto obj = rhs.TryAs<String>(); obj) {
    return ObjectHolder::Own(String(GetValue() + obj->GetValue()));
  }
  throw RuntimeError("Strings can be added to strings only");
}

bool String::Less(ObjectHolder rhs) {
  if (auto obj = rhs.TryAs<String>(); obj) {
    return GetValue() < obj->GetValue();
  }
  throw RuntimeError("Strings can be compared less to strings only");
}

bool String::Equal(ObjectHolder rhs) {
  if (auto obj = rhs.TryAs<String>(); obj) {
    return GetValue() == obj->GetValue();
  }
  throw RuntimeError("Strings can be compared equal to strings only");
}

bool Number::IsTrue() const {
  return GetValue() != 0;
}

ObjectHolder Number::Add(ObjectHolder rhs) {
  if (auto obj = rhs.TryAs<Number>(); obj) {
    return ObjectHolder::Own(Number(GetValue() + obj->GetValue()));
  }
  throw RuntimeError("Numbers can be added to numbers only");
}

ObjectHolder Number::Sub(ObjectHolder rhs) {
  if (auto obj = rhs.TryAs<Number>(); obj) {
    return ObjectHolder::Own(Number(GetValue() - obj->GetValue()));
  }
  throw RuntimeError("Numbers can be substituted with numbers only");
}

ObjectHolder Number::Mult(ObjectHolder rhs) {
  if (auto obj = rhs.TryAs<Number>(); obj) {
    return ObjectHolder::Own(Number(GetValue() * obj->GetValue()));
  }
  throw RuntimeError("Numbers can be multiplied by numbers only");
}

ObjectHolder Number::Div(ObjectHolder rhs) {
  if (auto obj = rhs.TryAs<Number>(); obj) {
    auto right = obj->GetValue();
    if (right == 0) {
      throw RuntimeError("Division by zero!");
    }
    return ObjectHolder::Own(Number(GetValue() / right));
  }
  throw RuntimeError("Numbers can be divided by numbers only");
}

bool Number::Less(ObjectHolder rhs) {
  if (auto obj = rhs.TryAs<Number>(); obj) {
    return GetValue() < obj->GetValue();
  }
  throw RuntimeError("Numbers can be compared less to numbers only");
}

bool Number::Equal(ObjectHolder rhs) {
  if (auto obj = rhs.TryAs<Number>(); obj) {
    return GetValue() == obj->GetValue();
  }
  throw RuntimeError("Numbers can be compared equal to numbers only");
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
  if (auto m = GetMethodHere(name); m) {
    return m;
  }

  if (parent_) {
    return parent_->GetMethod(name);
  }

  return nullptr;
}

void Class::Print(ostream& /* os */) {
  throw RuntimeError("Class::Print is not implemented yet"); // TODO maybe not needed?
}

bool Class::IsTrue() const {
  return true;
}

const Method* Class::GetMethodHere(const std::string& name) const {
  if (auto it = methods_.find(name); it != methods_.end()) {
    return &it->second;
  }
  return nullptr;
}

const std::string& Class::GetName() const {
  return name_;
}

void ClassInstance::Print(std::ostream& os) {
  if (HasMethod("__str__", 0)) {
    auto h = Call("__str__", {});
    h->Print(os);
  } else {
    os << this;
  }
}

bool ClassInstance::IsTrue() const {
  return true;
}

ObjectHolder ClassInstance::Add(ObjectHolder rhs) {
  return Call("__add__", {std::move(rhs)});
}

bool ClassInstance::HasMethod(const std::string& method, size_t argument_count) const {
  return CheckMethod(class_.GetMethod(method), argument_count);
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

ObjectHolder ClassInstance::Call(const std::string& method, const std::vector<ObjectHolder>& actual_args) {
  auto method_ptr = class_.GetMethod(method);
  if (!CheckMethod(method_ptr, actual_args.size())) {
    throw RuntimeError("There's no such method to call with this params!");
  }

  Closure c{{"self", ObjectHolder::Share(*this)}};
  for (size_t i = 0; i < actual_args.size(); ++i) {
    c[method_ptr->formal_params[i]] = std::move(actual_args[i]);
  }
  return method_ptr->body->Execute(c);
}

bool ClassInstance::CheckMethod(const Method* method_ptr, size_t argument_count) const {
  if (!method_ptr || !method_ptr->body) {
    return false;
  }
  // TODO check whether we need exact params count for actual params, or we can
  // have more than actual?
  return method_ptr->formal_params.size() == argument_count;
}

} /* namespace Runtime */
