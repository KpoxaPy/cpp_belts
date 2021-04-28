#include "statement.h"

#include <iostream>
#include <sstream>

#include "object.h"
#include "runtime_error.h"

using namespace std;

namespace Ast {

using Runtime::Closure;
using Runtime::RuntimeError;

ObjectHolder Assignment::Execute(Closure& /* closure */) {
  throw RuntimeError("Not implemented yet"); // FIXME
}

Assignment::Assignment(std::string /* var */, std::unique_ptr<Statement> /* rv */) {
  throw RuntimeError("Not implemented yet"); // FIXME
}

VariableValue::VariableValue(std::string /* var_name */) {
  throw RuntimeError("Not implemented yet"); // FIXME
}

VariableValue::VariableValue(std::vector<std::string> /* dotted_ids */) {
  throw RuntimeError("Not implemented yet"); // FIXME
}

ObjectHolder VariableValue::Execute(Closure& /* closure */) {
  throw RuntimeError("Not implemented yet"); // FIXME
}

unique_ptr<Print> Print::Variable(std::string /* var */) {
  throw RuntimeError("Not implemented yet"); // FIXME
}

Print::Print(unique_ptr<Statement> /* argument */) {
  throw RuntimeError("Not implemented yet"); // FIXME
}

Print::Print(vector<unique_ptr<Statement>> /* args */) {
  throw RuntimeError("Not implemented yet"); // FIXME
}

ObjectHolder Print::Execute(Closure& /* closure */) {
  throw RuntimeError("Not implemented yet"); // FIXME
}

ostream* Print::output = &cout;

void Print::SetOutputStream(ostream& output_stream) {
  output = &output_stream;
}

MethodCall::MethodCall(
  std::unique_ptr<Statement> /* object */
  , std::string /* method */
  , std::vector<std::unique_ptr<Statement>> /* args */
)
{
  throw RuntimeError("Not implemented yet"); // FIXME
}

ObjectHolder MethodCall::Execute(Closure& /* closure */) {
  throw RuntimeError("Not implemented yet"); // FIXME
}

ObjectHolder Stringify::Execute(Closure& /* closure */) {
  throw RuntimeError("Not implemented yet"); // FIXME
}

ObjectHolder Add::Execute(Closure& /* closure */) {
  throw RuntimeError("Not implemented yet"); // FIXME
}

ObjectHolder Sub::Execute(Closure& /* closure */) {
  throw RuntimeError("Not implemented yet"); // FIXME
}

ObjectHolder Mult::Execute(Runtime::Closure& /* closure */) {
  throw RuntimeError("Not implemented yet"); // FIXME
}

ObjectHolder Div::Execute(Runtime::Closure& /* closure */) {
  throw RuntimeError("Not implemented yet"); // FIXME
}

ObjectHolder Compound::Execute(Closure& /* closure */) {
  throw RuntimeError("Not implemented yet"); // FIXME
}

ObjectHolder Return::Execute(Closure& /* closure */) {
  throw RuntimeError("Not implemented yet"); // FIXME
}

ClassDefinition::ClassDefinition(ObjectHolder class_holder)
  : class_holder_(class_holder)
  , class_name_(class_holder_.TryAs<Runtime::Class>()->GetName())
{
}

ObjectHolder ClassDefinition::Execute(Runtime::Closure& /* closure */) {
  throw RuntimeError("Not implemented yet"); // FIXME
}

FieldAssignment::FieldAssignment(
  VariableValue object, std::string field_name, std::unique_ptr<Statement> rv
)
  : object(std::move(object))
  , field_name(std::move(field_name))
  , right_value(std::move(rv))
{
}

ObjectHolder FieldAssignment::Execute(Runtime::Closure& /* closure */) {
  throw RuntimeError("Not implemented yet"); // FIXME
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
  throw RuntimeError("Not implemented yet"); // FIXME
}

ObjectHolder Or::Execute(Runtime::Closure& /* closure */) {
  throw RuntimeError("Not implemented yet"); // FIXME
}

ObjectHolder And::Execute(Runtime::Closure& /* closure */) {
  throw RuntimeError("Not implemented yet"); // FIXME
}

ObjectHolder Not::Execute(Runtime::Closure& /* closure */) {
  throw RuntimeError("Not implemented yet"); // FIXME
}

Comparison::Comparison(
  Comparator /* cmp */, unique_ptr<Statement> /* lhs */, unique_ptr<Statement> /* rhs */
) {
  throw RuntimeError("Not implemented yet"); // FIXME
}

ObjectHolder Comparison::Execute(Runtime::Closure& /* closure */) {
  throw RuntimeError("Not implemented yet"); // FIXME
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
  throw RuntimeError("Not implemented yet"); // FIXME
}


} /* namespace Ast */
