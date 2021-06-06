#include "cell_impl.h"

#include <string_view>

using namespace std;

ICell::Value Cell::GetValue() const {
  CalculateValue();
  return cached_value_;
}

std::string Cell::GetText() const {
  return text_;
}

std::vector<Position> Cell::GetReferencedCells() const {
  throw runtime_error("unimplemented");
}

void Cell::SetText(std::string text) {
  text_ = std::move(text);
  // invalide cache & update refs
}

void Cell::CalculateValue() const {
  if (text_ != cached_text_) {
    if (text_.empty()) {
      cached_value_ = "";
    } else if (text_[0] == '\'') {
      string_view view = text_;
      view.remove_prefix(1);
      cached_value_ = string(view);
    } else if (text_[0] == '=') {
      // formula logic
      cached_value_ = 0.0;
    } else {
      cached_value_ = text_;
    }
    cached_text_ = text_;
  }
}