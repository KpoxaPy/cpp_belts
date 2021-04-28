#include "statement.h"

#include <iostream>
#include <sstream>

#include "object.h"
#include "runtime_error.h"

using namespace std;

namespace Ast {

using Runtime::Closure;
using Runtime::RuntimeError;

ObjectHolder Assignment::Execute(Closure& closure) {
  closure.insert_or_assign(var_name, right_value->Execute(closure));
  return closure[var_name];
}

Assignment::Assignment(std::string var, std::unique_ptr<Statement> rv)
  : var_name(std::move(var))
  , right_value(std::move(rv))
{
}

VariableValue::VariableValue(std::string var_name) {
  dotted_ids.push_back(std::move(var_name));
}

VariableValue::VariableValue(std::vector<std::string> dotted_ids)
  : dotted_ids(std::move(dotted_ids))
{
}

ObjectHolder VariableValue::Execute(Closure& closure) {
  ostringstream ss;

  const size_t last = dotted_ids.size() - 1;
  for (size_t i = 0; i <= last; ++i) {
    ss << (i == 0 ? "" : ".") << dotted_ids[i];

    auto it = closure.find(dotted_ids[i]);
    if (it == closure.end()) {
      break;
    }
    
    if (i == last) {
      return it->second;
    }

    if (auto inst = it->second.TryAs<Runtime::ClassInstance>(); inst) {
      closure = inst->Fields();
    }
  }

  throw RuntimeError("cannot find var " + ss.str());
}

unique_ptr<Print> Print::Variable(std::string var) {
  auto var_arg = make_unique<VariableValue>(var);
  return std::make_unique<Print>(std::move(var_arg));
}

Print::Print(unique_ptr<Statement> argument) {
  args_.push_back(std::move(argument));
}

Print::Print(vector<unique_ptr<Statement>> args)
  : args_(std::move(args))
{
}

ObjectHolder Print::Execute(Closure& closure) {
  bool was_first = false;
  for (auto& arg : args_) {
    if (was_first) {
      *output_ << " ";
    } else {
      was_first = true;
    }

    if (!arg) {
      throw RuntimeError("argument is malformed");
    }

    Runtime::Print(arg->Execute(closure), *output_);
  }
  *output_ << "\n";
  return ObjectHolder::None();
}

ostream* Print::output_ = &cout;

void Print::SetOutputStream(ostream& output_stream) {
  output_ = &output_stream;
}

MethodCall::MethodCall(
  std::unique_ptr<Statement> /* object */
  , std::string /* method */
  , std::vector<std::unique_ptr<Statement>> /* args */
)
{
  throw RuntimeError("MethodCall::MethodCall is not implemented yet"); // FIXME
}

ObjectHolder MethodCall::Execute(Closure& /* closure */) {
  throw RuntimeError("MethodCall::Execute is not implemented yet"); // FIXME
}

ObjectHolder Stringify::Execute(Closure& closure) {
  if (!argument) {
    throw RuntimeError("argument is malformed");
  }
  ostringstream ss;
  Runtime::Print(argument->Execute(closure), ss);
  return ObjectHolder::Own(Runtime::String(ss.str()));
}

ObjectHolder Add::Execute(Closure& closure) {
  if (!lhs || !rhs) {
    throw RuntimeError("arguments are malformed");
  }
  return Runtime::Add(lhs->Execute(closure), rhs->Execute(closure));
}

ObjectHolder Sub::Execute(Closure& closure) {
  if (!lhs || !rhs) {
    throw RuntimeError("arguments are malformed");
  }
  return Runtime::Sub(lhs->Execute(closure), rhs->Execute(closure));
}

ObjectHolder Mult::Execute(Closure& closure) {
  if (!lhs || !rhs) {
    throw RuntimeError("arguments are malformed");
  }
  return Runtime::Mult(lhs->Execute(closure), rhs->Execute(closure));
}

ObjectHolder Div::Execute(Closure& closure) {
  if (!lhs || !rhs) {
    throw RuntimeError("arguments are malformed");
  }
  return Runtime::Div(lhs->Execute(closure), rhs->Execute(closure));
}

ObjectHolder Compound::Execute(Closure& closure) {
  for (auto& stmt : statements_) {
    if (!stmt) {
      throw RuntimeError("Malformed statement");
    }
    stmt->Execute(closure);
  }

  return ObjectHolder::None(); // FIXME make execution flow
}

ObjectHolder Return::Execute(Closure& /* closure */) {
  throw RuntimeError("Return::Execute is not implemented yet"); // FIXME
}

ClassDefinition::ClassDefinition(ObjectHolder class_holder)
  : class_holder_(class_holder)
  , class_name_(class_holder_.TryAs<Runtime::Class>()->GetName())
{
}

ObjectHolder ClassDefinition::Execute(Runtime::Closure& /* closure */) {
  throw RuntimeError("ClassDefinition::Execute is not implemented yet"); // FIXME
}

FieldAssignment::FieldAssignment(
  VariableValue object, std::string field_name, std::unique_ptr<Statement> rv
)
  : object(std::move(object))
  , field_name(std::move(field_name))
  , right_value(std::move(rv))
{
}

ObjectHolder FieldAssignment::Execute(Runtime::Closure& closure) {
  
  if (auto holder = object.Execute(closure); holder) {
    if (auto inst = holder.TryAs<Runtime::ClassInstance>(); inst) {
      auto& fields = inst->Fields();
      fields.insert_or_assign(field_name, right_value->Execute(closure));
      return fields[field_name];
    }
  }
  throw RuntimeError("Cannot assign field " + field_name);
}

IfElse::IfElse(
  std::unique_ptr<Statement> /* condition */,
  std::unique_ptr<Statement> /* if_body */,
  std::unique_ptr<Statement> /* else_body */
)
{
  throw RuntimeError("Not implemented yet"); // FIXME
}

ObjectHolder IfElse::Execute(Runtime::Closure& /* closure */) {
  throw RuntimeError("IfElse::Execute is not implemented yet"); // FIXME
}

ObjectHolder Or::Execute(Runtime::Closure& /* closure */) {
  throw RuntimeError("Or::Execute is not implemented yet"); // FIXME
}

ObjectHolder And::Execute(Runtime::Closure& /* closure */) {
  throw RuntimeError("And::Execute is not implemented yet"); // FIXME
}

ObjectHolder Not::Execute(Runtime::Closure& /* closure */) {
  throw RuntimeError("Not::Execute is not implemented yet"); // FIXME
}

Comparison::Comparison(
  Comparator /* cmp */, unique_ptr<Statement> /* lhs */, unique_ptr<Statement> /* rhs */
) {
  throw RuntimeError("Comparison::Comparison is not implemented yet"); // FIXME
}

ObjectHolder Comparison::Execute(Runtime::Closure& /* closure */) {
  throw RuntimeError("Not::Execute is not implemented yet"); // FIXME
}

NewInstance::NewInstance(
  const Runtime::Class& class_, std::vector<std::unique_ptr<Statement>> args
)
  : class_(class_)
  , args(std::move(args))
{
}

NewInstance::NewInstance(const Runtime::Class& class_) : NewInstance(class_, {}) {
}

ObjectHolder NewInstance::Execute(Runtime::Closure& /* closure */) {
  return ObjectHolder::Own(Runtime::ClassInstance(class_));
  // FIXME add __init__ call
}


} /* namespace Ast */
