#include "cell_impl.h"

#include <queue>
#include <sstream>
#include <string_view>

#include "profile.h"
#include "set_utils.h"
#include "sheet_impl.h"

using namespace std;

Cell::Cell(Sheet& sheet)
  : sheet_(sheet)
  , cc_epoch_(sheet.cc_epoch)
{}

ICell::Value Cell::GetValue() const {
  METER_DURATION(sheet_.m_value);
  if (!value_) {
    if (text_.empty()) {
      value_ = "";
    } else if (text_[0] == kEscapeSign) {
      string_view view = text_;
      view.remove_prefix(1);
      value_ = string(view);
    } else if (formula_) {
      auto result = formula_->Evaluate(sheet_); // O(K)
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
  if (formula_) {
    ostringstream ss;
    ss << "=" << formula_->GetExpression();
    return ss.str();
  } else {
    return text_;
  }
}

std::vector<Position> Cell::GetReferencedCells() const {
  if (formula_) {
    return formula_->GetReferencedCells();
  }
  return {};
}

bool Cell::IsFree() const {
  return refs_from_.empty();
}

void Cell::SetText(std::string text) {
  METER_DURATION(sheet_.m_cell_set);
  if (text == text_) {
    return;
  }

  unique_ptr<IFormula> new_formula;
  if (text.size() > 1 && text[0] == kFormulaSign) {
    METER_DURATION(sheet_.m_cell_set_formula_parsing);
    string_view view = text;
    view.remove_prefix(1);
    new_formula = ParseFormula(string(view), sheet_);
  }

  ProcessRefs(new_formula.get());

  formula_ = std::move(new_formula);
  text_ = std::move(text);
  InvalidateCache();
}

void Cell::PrepareToDelete() {
  SetText("");
  for (auto ref : refs_from_) {
    ref->refs_to_.erase(this);
  }
}

void Cell::HandleInsertedRows(int before, int count) {
  if (formula_) {
    formula_->HandleInsertedRows(before, count);
  }
}

void Cell::HandleInsertedCols(int before, int count) {
  if (formula_) {
    formula_->HandleInsertedCols(before, count);
  }
}

void Cell::HandleDeletedRows(int first, int count) {
  if (formula_) {
    formula_->HandleDeletedRows(first, count);
  }
}

void Cell::HandleDeletedCols(int first, int count) {
  if (formula_) {
    formula_->HandleDeletedCols(first, count);
  }
}

Cell::Refs Cell::ProjectRefs(std::vector<Position> positions) {
  Refs refs;
  for (auto pos : positions) {
    refs.insert(&sheet_.InsertCell(pos));
  }
  return refs;
}

void Cell::ProcessRefs(IFormula* new_formula) {
  METER_DURATION(sheet_.m_cell_refs);
  auto [remove_refs, add_refs] = [this, new_formula]() {
    Refs new_refs;
    if (new_formula) {
      new_refs = ProjectRefs(new_formula->GetReferencedCells());
    }

    Refs old_refs;
    if (formula_) {
      old_refs = ProjectRefs(formula_->GetReferencedCells());
    }

    return SetTwoSideDifference(old_refs, new_refs);
  }();

  CheckCircular(add_refs);

  for (auto ref : remove_refs) {
    refs_to_.erase(ref);
    ref->refs_from_.erase(this);
  }

  for (auto ref : add_refs) {
    refs_to_.insert(ref);
    ref->refs_from_.insert(this);
  }
}

void Cell::CheckCircular(const Refs& refs) const {
  METER_DURATION(sheet_.m_cell_check_circular);

  ++sheet_.cc_epoch;
  const size_t cc_epoch = sheet_.cc_epoch;
  queue<Cell*> q;

  for (auto ref : refs) {
    if (ref == this) {
      throw CircularDependencyException("");
    }

    q.push(ref);
    ref->cc_epoch_ = cc_epoch;
  }

  if (refs_from_.empty()) {
    return;
  }

  while (!q.empty()) {
    auto ref = q.front();
    q.pop();

    if (ref == this) {
      throw CircularDependencyException("");
    }

    for (auto subref : ref->refs_to_) {
      if (subref->cc_epoch_ < cc_epoch) {
        q.push(subref);
        subref->cc_epoch_ = cc_epoch;
      }
    }
  }
}

void Cell::InvalidateCache() {
  METER_DURATION(sheet_.m_cell_invalidate);
  queue<Cell *> q;
  if (value_.has_value()) {
    q.push(this);
    value_.reset();
  }

  while (!q.empty()) {
    auto ref = q.front();
    q.pop();

    for (auto subref : ref->refs_from_) {
      if (subref->value_.has_value()) {
        q.push(subref);
        subref->value_.reset();
      }
    }
  }
}