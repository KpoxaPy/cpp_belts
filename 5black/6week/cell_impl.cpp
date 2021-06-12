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
    string_view view = text;
    view.remove_prefix(1);
    new_formula = ParseFormula(string(view));
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
  DurationMeter<microseconds> total;
  StatMeter<nanoseconds> m_processed_insert("Processed insert");
  StatMeter<nanoseconds> m_processed_check("Processed check");
  StatMeter<nanoseconds> m_queue_push("Queue push");
  StatMeter<nanoseconds> m_queue_pop("Queue pop");

  Refs processed;
  queue<Cell*> q;

  {
    METER_DURATION(sheet_.m_cell_check_circular);
    processed.reserve(2500);
    for (auto ref : refs) {
      {
        METER_DURATION(sheet_.m_cell_check_circular_queue_push);
        METER_DURATION(m_queue_push);
        q.push(ref);
      }

      {
        METER_DURATION(sheet_.m_cell_check_circular_proc_insert);
        METER_DURATION(m_processed_insert);
        processed.insert(ref);
      }
    }

    while (!q.empty()) {
      Cell* ref;
      {
        METER_DURATION(m_queue_pop);
        ref = q.front();
        q.pop();
      }

      if (ref == this) {
        throw CircularDependencyException("");
      }

      for (auto subref : ref->refs_to_) {
        bool not_processed;
        {
          METER_DURATION(sheet_.m_cell_check_circular_proc_check);
          METER_DURATION(m_processed_check);
          not_processed = processed.find(subref) == processed.end();
        }

        if (not_processed) {
          {
            METER_DURATION(sheet_.m_cell_check_circular_queue_push);
            METER_DURATION(m_queue_push);
            q.push(subref);
          }

          {
            METER_DURATION(sheet_.m_cell_check_circular_proc_insert);
            METER_DURATION(m_processed_insert);
            processed.insert(subref);
          }
        }
      }
    }
  }

  if (auto t = total.Get(); t > 35'000us) {
    cerr << "Cell::CheckCircular long duration\n";
    cerr << "\tTotal: " << t << "\n";
    cerr << "\tProcessed: " << processed.size() << "\n";
    cerr << "\t" << m_queue_push.Get() << "\n";
    cerr << "\t" << m_queue_pop.Get() << "\n";
    cerr << "\t" << m_processed_insert.Get() << "\n";
    cerr << "\t" << m_processed_check.Get() << "\n";
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