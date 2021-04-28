#include "object_holder.h"
#include "object.h"

namespace Runtime {

ObjectHolder ObjectHolder::Share(Object& object) {
  return ObjectHolder(std::shared_ptr<Object>(&object, [](auto*) { /* do nothing */ }));
}

ObjectHolder ObjectHolder::None() {
  return ObjectHolder();
}

Object& ObjectHolder::operator *() {
  return *Get();
}

const Object& ObjectHolder::operator *() const {
  return *Get();
}

Object* ObjectHolder::operator ->() {
  return Get();
}

const Object* ObjectHolder::operator ->() const {
  return Get();
}

Object* ObjectHolder::Get() {
  return data.get();
}

const Object* ObjectHolder::Get() const {
  return data.get();
}

ObjectHolder::operator bool() const {
  return Get();
}

bool IsTrue(ObjectHolder object) {
  if (!object) {
    return false;
  }

  return object->IsTrue();
}

void Print(ObjectHolder object, std::ostream& stream) {
  if (object) {
    object->Print(stream);
  } else {
    stream << "None";
  }
}

ObjectHolder Add(ObjectHolder lhs, ObjectHolder rhs) {
  if (!lhs || !rhs) {
    throw RuntimeError("Cannot operate with None");
  }

  return lhs->Add(rhs);
}

ObjectHolder Sub(ObjectHolder lhs, ObjectHolder rhs) {
  if (!lhs || !rhs) {
    throw RuntimeError("Cannot operate with None");
  }

  return lhs->Sub(rhs);
}

ObjectHolder Mult(ObjectHolder lhs, ObjectHolder rhs) {
  if (!lhs || !rhs) {
    throw RuntimeError("Cannot operate with None");
  }

  return lhs->Mult(rhs);
}

ObjectHolder Div(ObjectHolder lhs, ObjectHolder rhs) {
  if (!lhs || !rhs) {
    throw RuntimeError("Cannot operate with None");
  }

  return lhs->Div(rhs);
}

}
