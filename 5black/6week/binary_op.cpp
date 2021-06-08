#include "binary_op.h"

#include <cmath>
#include <ostream>

#include "formula_node_full.h"

using namespace std;

IFormula::Value BinaryOp::operator()(double lhs, double rhs) const {
  double result;
  if (op == '*') {
    result = lhs * rhs;
  } else if (op == '/') {
    result = lhs / rhs;
  } else if (op == '+') {
    result = lhs + rhs;
  } else if (op == '-') {
    result = lhs - rhs;
  } else {
    throw runtime_error("unknown op");
  }

  if (isfinite(result)) {
    return result;
  } else {
    return FormulaError(FormulaError::Category::Div0);
  }
}

ostream& operator<<(ostream& o, const BinaryOp& op) {
  bool l = false;
  bool r = false;

  if (op.op == '-') {
    if (holds_alternative<BinaryOp>(*op.rhs.get())) {
      char rhs_op = get<BinaryOp>(*op.rhs.get()).op;      
      if (rhs_op == '+' || rhs_op == '-') {
        r = true;
      }
    }
  }

  if (op.op == '*') {
    if (holds_alternative<BinaryOp>(*op.lhs.get())) {
      char lhs_op = get<BinaryOp>(*op.lhs.get()).op;      
      if (lhs_op == '+' || lhs_op == '-') {
        l = true;
      }
    }
    if (holds_alternative<BinaryOp>(*op.rhs.get())) {
      char rhs_op = get<BinaryOp>(*op.rhs.get()).op;      
      if (rhs_op == '+' || rhs_op == '-') {
        r = true;
      }
    }
  }

  if (op.op == '/') {
    if (holds_alternative<BinaryOp>(*op.lhs.get())) {
      char lhs_op = get<BinaryOp>(*op.lhs.get()).op;      
      if (lhs_op == '+' || lhs_op == '-') {
        l = true;
      }
    }

    if (holds_alternative<BinaryOp>(*op.rhs.get())) {
      r = true;
    }
  }

  return o
    << (l ? "(" : "") << *op.lhs.get() << (l ? ")" : "")
    << op.op
    << (r ? "(" : "") << *op.rhs.get() << (r ? ")" : "");
}

IFormula::Value EvaluateNode(const BinaryOp& op, const ISheet& sheet) {
  double lhs;
  {
    auto res = EvaluateNode(*op.lhs.get(), sheet);
    if (holds_alternative<FormulaError>(res)) {
      return res;
    }
    lhs = get<double>(res);
  }

  double rhs;
  {
    auto res = EvaluateNode(*op.rhs.get(), sheet);
    if (holds_alternative<FormulaError>(res)) {
      return res;
    }
    rhs = get<double>(res);
  }

  return op(lhs, rhs);
}
