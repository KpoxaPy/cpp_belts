#include <functional>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "cell_impl.h"
#include "common_etc.h"

class Sheet : public ISheet {
  using CellPtr = std::shared_ptr<Cell>;
  using Column = std::vector<CellPtr>;
  using Table = std::vector<Column>;

 public:
  void SetCell(Position pos, std::string text) override;
  const ICell* GetCell(Position pos) const override;
  ICell* GetCell(Position pos) override;
  Cell& InsertCell(Position pos);
  void ClearCell(Position pos) override;

  void InsertRows(int before, int count = 1) override;
  void InsertCols(int before, int count = 1) override;

  void DeleteRows(int first, int count = 1) override;
  void DeleteCols(int first, int count = 1) override;

  Size GetPrintableSize() const override;

  void PrintValues(std::ostream& output) const override;
  void PrintTexts(std::ostream& output) const override;

 private:
  Table table_;

  void ProcessNonEmptyCells(std::function<void(Position, Cell&)> f) const;
  Size GetSize() const;
};