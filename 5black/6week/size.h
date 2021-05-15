#pragma once

struct Size {
  int rows = 0;
  int cols = 0;

  bool operator==(const Size& rhs) const;
};