#pragma once

#include <memory>
#include <ostream>

#include "formula.h"
#include "formula_node_part.h"

struct BinaryOp {
  char op;
  std::unique_ptr<FormulaNode> lhs;
  std::unique_ptr<FormulaNode> rhs;

  IFormula::Value operator()(double lhs, double rhs) const;
};

std::ostream& operator<<(std::ostream& o, const BinaryOp& op);

IFormula::Value EvaluateNode(const BinaryOp& op, const ISheet& sheet);