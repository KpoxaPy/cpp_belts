#pragma once

#include <memory>
#include <ostream>

#include "formula.h"
#include "formula_node_part.h"

struct UnaryOp {
  char op;
  std::unique_ptr<FormulaNode> arg;

  IFormula::Value operator()(double arg) const;
};

std::ostream& operator<<(std::ostream& o, const UnaryOp& op);

IFormula::Value EvaluateNode(const UnaryOp& op, const ISheet& sheet);