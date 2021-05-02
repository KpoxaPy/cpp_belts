#pragma once

#include <istream>
#include <ostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace Json {

class Node;
using Array = std::vector<Node>;
using Map = std::map<std::string, Node>;

struct Bool {
  bool value;

  explicit Bool(bool v) : value(v) {}

  operator bool() const {
    return value;
  }
};

class Number : std::variant<double, int64_t> {
  friend class Printer;

 public:
  using variant::variant;

  Number(const std::string& s);
  Number(int v);
  Number(size_t v);

  template <typename T>
  operator T() const {
    if (std::holds_alternative<int64_t>(*this)) {
      return std::get<int64_t>(*this);
    } else if (std::holds_alternative<double>(*this)) {
      return std::get<double>(*this);
    }
    throw std::runtime_error("Json::Number holds something that out of domain");
  }
};

class Node : public std::variant<Array, Map, Number, std::string, Bool> {
  friend class Printer;

 public:
  using variant::variant;

  auto& AsArray() {
    return std::get<Array>(*this);
  }
  const auto& AsArray() const {
    return std::get<Array>(*this);
  }

  auto& AsMap() {
    return std::get<Map>(*this);
  }
  const auto& AsMap() const {
    return std::get<Map>(*this);
  }

  const auto& AsNumber() const {
    return std::get<Number>(*this);
  }

  auto& AsString() {
    return std::get<std::string>(*this);
  }
  const auto& AsString() const {
    return std::get<std::string>(*this);
  }

  bool AsBool() const {
    return std::get<Bool>(*this);
  }
};

std::ostream& operator<<(std::ostream& s, const Node& node);

class Printer {
  friend std::ostream& operator<<(std::ostream& s, const Printer& printer);

 public:
  Printer(const Node& node, bool pretty_print = true);

 private:
  Printer(const Node& node, bool pretty_print, size_t level);

  std::string PrettySpacer(bool inside = true) const;
  std::string PrettyEndline() const;

  void Print(std::ostream& s, const Node& node) const;
  void Print(std::ostream& s, const Number& number) const;

  const Node& node_;
  bool pretty_print_;
  size_t level_;
};

std::ostream& operator<<(std::ostream& s, const Printer& printer);

class Document {
 public:
  explicit Document(Node root);

  const Node& GetRoot() const;

 private:
  Node root;
};

Document Load(std::istream& input);

}  // namespace Json
