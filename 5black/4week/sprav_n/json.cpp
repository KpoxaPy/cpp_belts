#include "json.h"

#include <stdexcept>
#include <sstream>
#include <iomanip>

using namespace std;

namespace Json {

Number::Number(const string& s)
{
  if (s.find('.') != string::npos) {
    *this = stod(s);
  } else {
    *this = stoi(s);
  }
}

Number::Number(int v) {
  *this = static_cast<int64_t>(v);
}

Number::Number(size_t v) {
  *this = static_cast<int64_t>(v);
}

std::ostream& operator<<(std::ostream& s, const Node& node) {
  return s << Printer(node);
}

Printer::Printer(const Node& node, bool pretty_print)
  : node_(node)
  , pretty_print_(pretty_print)
  , level_(0)
{}

Printer::Printer(const Node& node, bool pretty_print, size_t level)
  : node_(node)
  , pretty_print_(pretty_print)
  , level_(level)
{}

std::string Printer::PrettySpacer(bool inside) const {
  ostringstream ss;
  if (pretty_print_) {
    for (size_t i = 0; i < level_ + (inside ? 1 : 0); ++i) {
      ss << "  ";
    }
  }
  return ss.str();
}

std::string Printer::PrettyEndline() const {
  if (pretty_print_) {
    return "\n";
  } else {
    return {};
  }
}

void Printer::Print(std::ostream& s, const Node& node) const {
  if (holds_alternative<Array>(node)) {
    s << "[" << PrettyEndline();
    auto array = node.AsArray();
    for (auto it = array.begin(); it != array.end(); ++it) {
      s << PrettySpacer() << Printer(*it, pretty_print_, level_ + 1)
        << (next(it) != array.end() ? "," : "") << PrettyEndline();
    }
    s << PrettySpacer(false) << "]";
  } else if (holds_alternative<Map>(node)) {
    s << "{" << PrettyEndline();
    auto map = node.AsMap();
    for (auto it = map.begin(); it != map.end(); ++it) {
      s << PrettySpacer() << quoted(it->first) << ":" << (pretty_print_ ? " " : "") << Printer(it->second, pretty_print_, level_ + 1)
        << (next(it) != map.end() ? "," : "") << PrettyEndline();
    }
    s << PrettySpacer(false) << "}";
  } else if (holds_alternative<Number>(node)) {
    Print(s, node.AsNumber());
  } else if (holds_alternative<std::string>(node)) {
    s << quoted(node.AsString());
  } else if (holds_alternative<Bool>(node)) {
    s << (node.AsBool() ? "true" : "false");
  }
}

void Printer::Print(std::ostream& s, const Number& number) const {
  if (holds_alternative<double>(number)) {
    s << setprecision(10) << get<double>(number);
  } else if (holds_alternative<int64_t>(number)) {
    s << get<int64_t>(number);
  }

}

ostream& operator<<(ostream& s, const Printer& p) {
  p.Print(s, p.node_);
  return s;
}

Document::Document(Node root)
  : root(move(root))
{}

const Node& Document::GetRoot() const {
  return root;
}

Node LoadNode(istream& input);

Node LoadArray(istream& input) {
  vector<Node> result;

  for (char c; input >> c && c != ']';) {
    if (c != ',') {
      input.putback(c);
    }
    result.push_back(LoadNode(input));
  }

  return Node(move(result));
}

Node LoadNumber(string input) {
  return Number(move(input));
}

Node LoadString(istream& input) {
  string line;
  getline(input, line, '"');
  return Node(move(line));
}

Node LoadDict(istream& input) {
  map<string, Node> result;

  for (char c; input >> c && c != '}';) {
    if (c == ',') {
      input >> c;
    }

    string key = LoadString(input).AsString();
    input >> c;
    result.emplace(move(key), LoadNode(input));
  }

  return Node(move(result));
}

Node LoadLiteral(istream& input) {
  ostringstream ss;
  char c;
  while (input >> c && c != '}' && c != ']' && c != ',' && c != ':') {
    ss << c;
  }
  if (input) {
    input.putback(c);
  }

  if (ss.str() == "true") {
    return Bool(true);
  } else if (ss.str() == "false") {
    return Bool(false);
  } else {
    return LoadNumber(ss.str());
  }
}

Node LoadNode(istream& input) {
  char c;
  input >> c;

  if (c == '[') {
    return LoadArray(input);
  } else if (c == '{') {
    return LoadDict(input);
  } else if (c == '"') {
    return LoadString(input);
  } else {
    input.putback(c);
    return LoadLiteral(input);
  }
}

Document Load(istream& input) {
  return Document{LoadNode(input)};
}

}  // namespace Json
