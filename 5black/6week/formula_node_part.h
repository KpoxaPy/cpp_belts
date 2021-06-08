#pragma once

#include <variant>

#include "formula_error.h"
#include "position.h"

class BinaryOp;
class UnaryOp;
using FormulaNode = std::variant<double, BinaryOp, UnaryOp, Position, FormulaError>;