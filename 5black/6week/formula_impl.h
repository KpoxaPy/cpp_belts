#pragma once

#include <vector>

#include "formula.h"
#include "formula_node_full.h"
#include "position.h"

class Formula : public IFormula {
 public:
  void SetReferencedCells(std::vector<Position> refs);

  void SetRootNode(FormulaNode root);

  Value Evaluate(const ISheet& sheet) const override;

  std::string GetExpression() const override;

  std::vector<Position> GetReferencedCells() const override;

  HandlingResult HandleInsertedRows(int before, int count) override;
  HandlingResult HandleInsertedCols(int before, int count) override;

  HandlingResult HandleDeletedRows(int first, int count) override;

  HandlingResult HandleDeletedCols(int first, int count) override;

 private:
  std::vector<Position> referenced_cells_;
  FormulaNode root_;
};
