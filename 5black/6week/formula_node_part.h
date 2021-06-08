#pragma once

#include <variant>

class BinaryOp;
class UnaryOp;
using FormulaNode = std::variant<double, BinaryOp, UnaryOp, Position>;