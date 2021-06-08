#include "cell_impl.h"

#include <queue>
#include <string_view>

#include "sheet_impl.h"
#include "exception.h"

using namespace std;

Cell::Cell(Sheet& sheet)
  : sheet_(sheet)
{}

ICell::Value Cell::GetValue() const {
  if (!value_) {
    if (text_.empty()) {
      value_ = "";
    } else if (text_[0] == '\'') {
      string_view view = text_;
      view.remove_prefix(1);
      value_ = string(view);
    } else if (formula_ && formula_.value()) {
      auto result = formula_.value()->Evaluate(sheet_);
      if (holds_alternative<double>(result)) {
        value_ = get<double>(result);
      } else if (holds_alternative<FormulaError>(result)) {
        value_ = get<FormulaError>(result);
      } else {
        value_ = 0.0;
      }
    } else {
      value_ = text_;
    }
  }

  return *value_;
}

std::string Cell::GetText() const {
  if (formula_ && formula_.value()) {
    return formula_.value()->GetExpression();
  } else {
    return text_;
  }
}

std::vector<Position> Cell::GetReferencedCells() const {
  if (formula_) {
    return formula_.value()->GetReferencedCells();
  }
  return {};
}

const Cell::RefsTo& Cell::GetReferencingCells() const {
  return refs_to_;
}

void Cell::SetText(std::string text) {
  if (text == text_) {
    return;
  }

  unique_ptr<IFormula> new_formula;
  if (text.size() > 1 && text[0] == '=') {
    string_view view = text;
    view.remove_prefix(1);
    new_formula = ParseFormula(string(view));
  }

  auto all_refs = GetAllReferencingCells();

  // check circular
  if (new_formula) {
    for (auto& pos : new_formula->GetReferencedCells()) {
      Cell* ptr = dynamic_cast<Cell*>(sheet_.GetCell(pos));
      if (ptr && all_refs.find(ptr) != all_refs.end()) {
        throw CircularDependencyException("");
      }
    }
  }

  PropagadeRefsTo(false);
  if (new_formula) {
    formula_ = std::move(new_formula);
  } else if (formula_) {
    formula_.reset();
  }
  PropagadeRefsTo(true);

  text_ = std::move(text);
  value_.reset();
  for (Cell* ptr : all_refs) {
    ptr->value_.reset();
  }
}

void Cell::PropagadeRefsTo(bool add) {
  if (!formula_ || !formula_.value()) {
    return;
  }

  for (auto& ref_pos : formula_.value()->GetReferencedCells()) {
    auto& cell = sheet_.GetReferencedCell(ref_pos);
    if (add) {
      cell.AddRefTo(this);
    } else {
      cell.ClearRefTo(this);
    }
  }
}

void Cell::AddRefTo(Cell* cell_ptr) {
  refs_to_.insert(cell_ptr);
}

void Cell::ClearRefTo(Cell* cell_ptr) {
  refs_to_.erase(cell_ptr);
}

Cell::RefsTo Cell::GetAllReferencingCells() {
  unordered_set<Cell *> refs;
  queue<Cell *> q;
  q.push(this);

  while (!q.empty()) {
    for (Cell* cell_ptr : q.front()->refs_to_) {
      auto [_, is_inserted] = refs.insert(cell_ptr);
      if (is_inserted) {
        q.push(cell_ptr);
      }
    }
    q.pop();
  }

  return refs;
}