#pragma once

#include <memory>
#include <optional>
#include <unordered_set>

#include "common_etc.h"

class Sheet;

class Cell : public ICell {
  using Refs = std::unordered_set<Cell*>;

 public:
  Cell(Sheet& sheet);

  Value GetValue() const override; // O(K), O(1)
  std::string GetText() const override; // O(1)

  std::vector<Position> GetReferencedCells() const override; // O(1)
  bool IsFree() const; // O(1)
  void SetText(std::string text); // O(K*R), O(1)
  void PrepareToDelete(); // O(K*R)

  void HandleInsertedRows(int before, int count = 1);
  void HandleInsertedCols(int before, int count = 1);

  void HandleDeletedRows(int first, int count = 1);
  void HandleDeletedCols(int first, int count = 1);

 private:
  Sheet& sheet_;
  Refs refs_to_;
  Refs refs_from_;
  size_t cc_epoch_ = 0;

  std::string text_;
  std::unique_ptr<IFormula> formula_;

  mutable std::optional<Value> value_;

  Refs ProjectRefs(std::vector<Position> positions);
  void ProcessRefs(IFormula* new_formula); // O(K*R)
  void CheckCircular(const Refs& refs) const;
  void CheckCircularPart(const Cell* ref) const;
  void InvalidateCache(); // O(K)
};