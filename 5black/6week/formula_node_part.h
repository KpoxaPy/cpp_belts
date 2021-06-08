#pragma once

#include <variant>

#include "common_etc.h"

class BinaryOp;
class UnaryOp;
using FormulaNode = std::variant<double, BinaryOp, UnaryOp, Position, FormulaError>;