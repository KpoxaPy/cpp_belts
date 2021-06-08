#include "formula_error.h"

#include <stdexcept>

using namespace std;

namespace {

const string RefStr = "#REF!";
const string RefValue = "#VALUE!";
const string RefDiv0 = "#DIV/0!";

}

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
  switch (category_) {
  case Category::Ref:
    return RefStr;
  case Category::Value:
    return RefValue;
  case Category::Div0:
    return RefDiv0;
  }
  return "";
}

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
  output << fe.ToString();
  return output;
}