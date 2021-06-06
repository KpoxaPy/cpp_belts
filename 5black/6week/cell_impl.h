#pragma once

#include "cell.h"

class Cell : public ICell {
 public:
  Value GetValue() const override;
  std::string GetText() const override;

  std::vector<Position> GetReferencedCells() const override;

  void SetText(std::string text);

 private:
  std::string text_;
  mutable std::string cached_text_;
  mutable Cell::Value cached_value_ = "";

  void CalculateValue() const;
};