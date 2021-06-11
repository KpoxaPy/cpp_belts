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
{}

ICell::Value Cell::GetValue() const {
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
  if (text == text_) {
    return;
  }

  DurationMeter<microseconds> total;
  DurationMeter<microseconds> dur;

  dur.Start();
  unique_ptr<IFormula> new_formula;
  if (text.size() > 1 && text[0] == kFormulaSign) {
    string_view view = text;
    view.remove_prefix(1);
    new_formula = ParseFormula(string(view));
  }
  auto t_formula = dur.Get();

  dur.Start();
  ProcessRefs(new_formula.get());
  auto t_refs = dur.Get();

  formula_ = std::move(new_formula);
  text_ = std::move(text);
  dur.Start();
  InvalidateCache();
  auto t_invalidate = dur.Get();

  if (auto t = total.Get(); t > 50'000us) {
    cerr << "Cell::SetText long duration\n";
    cerr << "\tTotal: " << t << "\n";
    cerr << "\tFormula: " << t_formula << "\n";
    cerr << "\tRefs: " << t_refs << "\n";
    cerr << "\tCache: " << t_invalidate << "\n";
  }
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
  DurationMeter<microseconds> total;
  DurationMeter<microseconds> dur;

  dur.Start();
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
  auto t_project = dur.Get();

  dur.Start();
  CheckCircular(add_refs);
  auto t_circ = dur.Get();

  dur.Start();
  for (auto ref : remove_refs) {
    refs_to_.erase(ref);
    ref->refs_from_.erase(this);
  }

  for (auto ref : add_refs) {
    refs_to_.insert(ref);
    ref->refs_from_.insert(this);
  }
  auto t_graph = dur.Get();

  if (auto t = total.Get(); t > 50'000us) {
    cerr << "Cell::ProcessRefs long duration\n";
    cerr << "\tTotal: " << t << "\n";
    cerr << "\tProject: " << t_project << "\n";
    cerr << "\tCheckCircular: " << t_circ << "\n";
    cerr << "\tGraph: " << t_graph << "\n";
  }
}

void Cell::CheckCircular(const Refs& refs) const {
  DurationMeter<milliseconds> dur;

  Refs processed;
  queue<Cell *> q;
  for (auto ref : refs) {
    q.push(ref);
    processed.insert(ref);
  }

  while (!q.empty()) {
    auto ref = q.front();
    q.pop();

    if (ref == this) {
      throw CircularDependencyException("");
    }

    for (auto subref : ref->refs_to_) {
      if (processed.find(subref) == processed.end()) {
        q.push(subref);
        processed.insert(subref);
      }
    }
  }
}

void Cell::InvalidateCache() {
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