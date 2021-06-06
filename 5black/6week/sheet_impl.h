#include <memory>
#include <unordered_map>

#include "cell_impl.h"
#include "sheet.h"

class Sheet : public ISheet {
 public:
  Sheet();
  ~Sheet();

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
  Position printable_start_;
  Size printable_size_;

  std::unordered_map<Position, std::unique_ptr<Cell>> cells_;
};