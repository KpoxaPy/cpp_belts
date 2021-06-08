#pragma once

#include <ostream>

#include "formula.h"
#include "formula_node_part.h"
#include "position.h"

std::ostream& operator<<(std::ostream& o, const FormulaNode& node);

IFormula::Value EvaluateNode(const FormulaNode& node, const ISheet& sheet);
IFormula::Value EvaluateNode(double val, const ISheet& sheet);
IFormula::Value EvaluateNode(const Position& pos, const ISheet& sheet);