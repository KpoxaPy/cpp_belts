#pragma once

#include <string>
#include <string_view>

// Позиция ячейки. Индексация с нуля.
struct Position {
  int row = 0;
  int col = 0;

  bool operator==(const Position& rhs) const;
  bool operator<(const Position& rhs) const;

  bool IsValid() const;
  std::string ToString() const;

  static Position FromString(std::string_view str);

  static const int kMaxRows = 16384;
  static const int kMaxCols = 16384;
};