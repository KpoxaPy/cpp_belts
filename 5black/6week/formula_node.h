#pragma once

#include <ostream>

#include "formula.h"
#include "formula_node_part.h"

std::ostream& operator<<(std::ostream& o, const FormulaNode& node);
IFormula::Value EvaluateNode(const FormulaNode& node, const ISheet& sheet);