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
  Closure* closure_ptr = &closure;
  ostringstream ss;

  const size_t last = dotted_ids.size() - 1;
  for (size_t i = 0; i <= last; ++i) {
    ss << (i == 0 ? "" : ".") << dotted_ids[i];

    auto it = closure_ptr->find(dotted_ids[i]);
    if (it == closure_ptr->end()) {
      break;
    }
    
    if (i == last) {
      return it->second;
    }

    if (auto inst = it->second.TryAs<Runtime::ClassInstance>(); inst) {
      closure_ptr = &inst->Fields();
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
  std::unique_ptr<Statement> object
  , std::string method
  , std::vector<std::unique_ptr<Statement>> args
)
  : object(std::move(object))
  , method(std::move(method))
  , args(std::move(args))
{
}

ObjectHolder MethodCall::Execute(Closure& closure) {
  auto object_holder = object->Execute(closure);
  if (!object_holder) {
    throw RuntimeError("Cannot call method, object is None");
  }

  auto inst = object_holder.TryAs<Runtime::ClassInstance>();
  if (!inst) {
    throw RuntimeError("Cannot call method, object is not class instance");
  }

  std::vector<ObjectHolder> actual_args;
  actual_args.reserve(args.size());
  for (auto& arg : args) {
    actual_args.emplace_back(arg->Execute(closure));
  }
  auto h = inst->Call(method, actual_args);
  h.SetReturnState(false);
  return h;
}

ObjectHolder Stringify::Execute(Closure& closure) {
  ostringstream ss;
  Runtime::Print(argument->Execute(closure), ss);
  return ObjectHolder::Own(Runtime::String(ss.str()));
}

ObjectHolder Add::Execute(Closure& closure) {
  return Runtime::Add(lhs->Execute(closure), rhs->Execute(closure));
}

ObjectHolder Sub::Execute(Closure& closure) {
  return Runtime::Sub(lhs->Execute(closure), rhs->Execute(closure));
}

ObjectHolder Mult::Execute(Closure& closure) {
  return Runtime::Mult(lhs->Execute(closure), rhs->Execute(closure));
}

ObjectHolder Div::Execute(Closure& closure) {
  return Runtime::Div(lhs->Execute(closure), rhs->Execute(closure));
}

ObjectHolder Compound::Execute(Closure& closure) {
  for (auto& stmt : statements_) {
    if (!stmt) {
      throw RuntimeError("Malformed statement");
    }
    auto h = stmt->Execute(closure);
    if (h.IsReturnState()) {
      return h;
    }
  }

  return ObjectHolder::None();
}

ObjectHolder Return::Execute(Closure& closure) {
  auto h = statement_->Execute(closure);
  h.SetReturnState(true);
  return h;
}

ClassDefinition::ClassDefinition(ObjectHolder class_holder)
  : class_holder_(class_holder)
  , class_name_(class_holder_.TryAs<Runtime::Class>()->GetName())
{
}

ObjectHolder ClassDefinition::Execute(Runtime::Closure&) {
  return class_holder_;
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
  std::unique_ptr<Statement> condition,
  std::unique_ptr<Statement> if_body,
  std::unique_ptr<Statement> else_body
)
  : condition_(std::move(condition))
  , if_body_(std::move(if_body))
  , else_body_(std::move(else_body))
{
  if (!condition_ || !if_body_) {
    throw RuntimeError("Malformed condition or if_body");
  }
}

ObjectHolder IfElse::Execute(Runtime::Closure& closure) {
  auto cond_res_holder = condition_->Execute(closure);
  if (auto cond = cond_res_holder->IsTrue(); cond) {
    return if_body_->Execute(closure);
  }
  if (else_body_) {
    return else_body_->Execute(closure);
  }
  return ObjectHolder::None();
}

ObjectHolder Or::Execute(Runtime::Closure& closure) {
  return Runtime::Or(lhs->Execute(closure), rhs->Execute(closure));
}

ObjectHolder And::Execute(Runtime::Closure& closure) {
  return Runtime::And(lhs->Execute(closure), rhs->Execute(closure));
}

ObjectHolder Not::Execute(Runtime::Closure& closure) {
  return Runtime::Not(argument->Execute(closure));
}

Comparison::Comparison(
  Comparator cmp, unique_ptr<Statement> lhs, unique_ptr<Statement> rhs
)
  : comparator_(std::move(cmp))
  , left_(std::move(lhs))
  , right_(std::move(rhs))
{
  if (!left_ || !right_) {
    throw RuntimeError("arguments are malformed");
  }
}

ObjectHolder Comparison::Execute(Runtime::Closure& closure) {
  bool result = comparator_(left_->Execute(closure), right_->Execute(closure));
  return ObjectHolder::Own(Runtime::Bool{result});
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

ObjectHolder NewInstance::Execute(Runtime::Closure& closure) {
  auto h = ObjectHolder::Own(Runtime::ClassInstance(class_));
  auto inst = h.TryAs<Runtime::ClassInstance>();

  std::vector<ObjectHolder> actual_args;
  actual_args.reserve(args.size());
  for (auto& arg : args) {
    actual_args.emplace_back(arg->Execute(closure));
  }

  if (inst->HasMethod("__init__", args.size())) {
    inst->Call("__init__", actual_args);
  }

  return h;
}


} /* namespace Ast */
