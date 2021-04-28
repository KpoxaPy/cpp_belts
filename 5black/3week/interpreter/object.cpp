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

bool Number::IsTrue() const {
  return GetValue() == 0;
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
  throw RuntimeError("Not implemented yet"); // FIXME
}

bool Class::IsTrue() const {
  return true;
}

const std::string& Class::GetName() const {
  return name_;
}

void ClassInstance::Print(std::ostream& /* os */) {
  throw RuntimeError("Not implemented yet"); // FIXME
}

bool ClassInstance::IsTrue() const {
  return true;
}

bool ClassInstance::HasMethod(const std::string& /* method */, size_t /* argument_count */) const {
  throw RuntimeError("Not implemented yet"); // FIXME
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
  throw RuntimeError("Not implemented yet"); // FIXME
}

} /* namespace Runtime */
