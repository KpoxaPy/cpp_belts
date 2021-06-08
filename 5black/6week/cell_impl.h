#pragma once

#include <memory>
#include <optional>
#include <unordered_set>

#include "common_etc.h"

class Sheet;

class Cell : public ICell {
  using RefsTo = std::unordered_set<Cell*>;

 public:
  Cell(Sheet& sheet);

  Value GetValue() const override;
  std::string GetText() const override;
  IFormula* GetFormula();

  std::vector<Position> GetReferencedCells() const override;
  const RefsTo& GetReferencingCells() const;

  void SetText(std::string text);
  void PropagadeRefsTo(bool add);

 private:
  Sheet& sheet_;

  std::string text_;
  std::optional<std::unique_ptr<IFormula>> formula_;
  RefsTo refs_to_;

  mutable std::optional<Value> value_;

  void AddRefTo(Cell* cell_ptr);
  void ClearRefTo(Cell* cell_ptr);

  RefsTo GetAllReferencingCells();
};