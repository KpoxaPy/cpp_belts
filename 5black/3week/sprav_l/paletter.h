#pragma once

#include "sprav_mapper.h"

class Paletter {
 public:
  Paletter(const SpravMapper& mapper);

  const Svg::Color& operator()() const;

 private:
  const SpravMapper& mapper_;
  mutable std::size_t idx = 0;
};