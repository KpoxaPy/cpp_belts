#include <memory>
#include <vector>

#include "cell_impl.h"
#include "sheet.h"

class Sheet : public ISheet {
  using Column = std::vector<std::unique_ptr<Cell>>;
  using Table = std::vector<Column>;

 public:
  Sheet();

  const Cell* GetSpecificCell(Position pos) const;
  Cell* GetSpecificCell(Position pos);
  Cell& GetReferencedCell(Position pos);

  void SetCell(Position pos, std::string text) override;
  const ICell* GetCell(Position pos) const override;
  ICell* GetCell(Position pos) override;
  void ClearCell(Position pos) override;

  void InsertRows(int before, int count = 1) override;
  void InsertCols(int before, int count = 1) override;

  void DeleteRows(int first, int count = 1) override;
  void DeleteCols(int first, int count = 1) override;

  Size GetPrintableSize() const override;

  void PrintValues(std::ostream& output) const override;
  void PrintTexts(std::ostream& output) const override;

 private:
  Size size_ = {0, 0};
  Table table_;

  void AddCellToPrintable(Position pos);
  Cell& InsertCell(Position pos);
};