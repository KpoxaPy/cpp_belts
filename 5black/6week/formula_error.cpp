#include "formula_error.h"

#include <stdexcept>

using namespace std;

FormulaError::FormulaError(Category category)
  : category_(category)
{}

FormulaError::Category FormulaError::GetCategory() const {
  return category_;
}

bool FormulaError::operator==(FormulaError rhs) const {
  return category_ == rhs.category_;
}

std::string_view FormulaError::ToString() const {
  throw runtime_error("unimplemented");
}

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
  throw runtime_error("unimplemented");
}