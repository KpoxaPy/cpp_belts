#include "formula_impl.h"

#include <sstream>

using namespace std;

void Formula::SetReferencedCells(vector<Position> refs) {
  referenced_cells_ = std::move(refs);
}

void Formula::SetRootNode(FormulaNode root) {
  root_ = std::move(root);
}

IFormula::Value Formula::Evaluate(const ISheet& sheet) const {
  return EvaluateNode(root_, sheet);
}

std::string Formula::GetExpression() const {
  ostringstream ss;
  ss << root_;
  return ss.str();
}

std::vector<Position> Formula::GetReferencedCells() const {
  return referenced_cells_;
}

IFormula::HandlingResult Formula::HandleInsertedRows(int before, int count) {
  return HandlingResult::NothingChanged;
}

IFormula::HandlingResult Formula::HandleInsertedCols(int before, int count) {
  return HandlingResult::NothingChanged;
}

IFormula::HandlingResult Formula::HandleDeletedRows(int first, int count) {
  return HandlingResult::NothingChanged;
}

IFormula::HandlingResult Formula::HandleDeletedCols(int first, int count) {
  return HandlingResult::NothingChanged;
}