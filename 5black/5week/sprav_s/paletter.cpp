#include "paletter.h"

Paletter::Paletter(const SpravMapper& mapper)
  : mapper_(mapper)
{}

const Svg::Color& Paletter::operator()() const {
  const auto& palette = mapper_.GetSettings().color_palette;
  if (palette.size() == 0) {
    return Svg::NoneColor;
  }
  return palette[idx++ % palette.size()];
}