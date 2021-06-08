#include "unary_op.h"

#include "formula_node_full.h"

using namespace std;

IFormula::Value UnaryOp::operator()(double arg) const {
  if (op == '+') {
    return arg;
  } else if (op == '-') {
    return -arg;
  }
  throw runtime_error("unknown op");
}

ostream& operator<<(ostream& o, const UnaryOp& op) {
  bool need = false;

  if (holds_alternative<BinaryOp>(*op.arg.get())) {
    char rhs_op = get<BinaryOp>(*op.arg.get()).op;      
    if (rhs_op == '+' || rhs_op == '-') {
      need = true;
    }
  }

  return o
    << op.op
    << (need ? "(" : "") << *op.arg.get() << (need ? ")" : "");
}

IFormula::Value EvaluateNode(const UnaryOp& op, const ISheet& sheet) {
  double arg;
  {
    auto res = EvaluateNode(*op.arg.get(), sheet);
    if (holds_alternative<FormulaError>(res)) {
      return res;
    }
    arg = get<double>(res);
  }

  return op(arg);
}