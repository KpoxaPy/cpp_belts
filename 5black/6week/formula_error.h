#pragma once

#include <string_view>
#include <ostream>

// Описывает ошибки, которые могут возникнуть при вычислении формулы.
class FormulaError {
public:
  enum class Category {
    Ref,  // ссылка на несуществующую ячейку (например, она была удалена)
    Value,  // ячейка не может быть трактована как число
    Div0,  // в результате вычисления возникло деление на ноль
  };

  FormulaError(Category category);

  Category GetCategory() const;

  bool operator==(FormulaError rhs) const;

  std::string_view ToString() const;

private:
  Category category_;
};

std::ostream& operator<<(std::ostream& output, FormulaError fe);