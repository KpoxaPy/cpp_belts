#pragma once

#include <stdexcept>

// Исключение, выбрасываемое при попытке передать в метод некорректную позицию
class InvalidPositionException : public std::out_of_range {
public:
  using std::out_of_range::out_of_range;
};

// Исключение, выбрасываемое при попытке задать синтаксически некорректную
// формулу
class FormulaException : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

// Исключение, выбрасываемое при попытке задать формулу, которая приводит к
// циклической зависимости между ячейками
class CircularDependencyException : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

// Исключение, выбрасываемое если вставка строк/столбцов в таблицу приведёт к
// ячейке с позицией больше максимально допустимой
class TableTooBigException : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};