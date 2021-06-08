#include "formula_node_full.h"

using namespace std;

std::ostream& operator<<(std::ostream& o, const Position& p) {
  return o << p.ToString();
}

ostream& operator<<(ostream& o, const FormulaNode& node) {
  visit([&o](auto&& val) {
    o << val;
  }, node);
  return o;
}

IFormula::Value EvaluateNode(const FormulaNode& node, const ISheet& sheet) {
  return visit([&sheet](auto&& val) {
    return EvaluateNode(val, sheet);
  }, node);
}

IFormula::Value EvaluateNode(double val, const ISheet& sheet) {
  return val;
}

IFormula::Value EvaluateNode(const Position& pos, const ISheet& sheet) {
  auto cell_ptr = sheet.GetCell(pos);
  if (!cell_ptr) {
    return 0.0;
  }

  auto value = cell_ptr->GetValue();
  if (holds_alternative<double>(value)) {
    return get<double>(value);
  }

  if (holds_alternative<string>(value)) {
    string& text = get<string>(value);
    if (text.empty()) {
      return 0.0;
    }

    try {
      size_t pos;
      if (double val = stod(text, &pos); pos == text.size()) {
        return val;
      }
      return FormulaError(FormulaError::Category::Value);
    } catch (...) {
      return FormulaError(FormulaError::Category::Value);
    }
  }

  if (holds_alternative<FormulaError>(value)) {
    return get<FormulaError>(value);
  }

  throw runtime_error("undecideable value");
}

