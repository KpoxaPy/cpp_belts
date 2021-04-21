#include "lexer.h"

#include <algorithm>
#include <charconv>
#include <unordered_map>

using namespace std;

namespace Parse {

bool operator == (const Token& lhs, const Token& rhs) {
  using namespace TokenType;

  if (lhs.index() != rhs.index()) {
    return false;
  }
  if (lhs.Is<Char>()) {
    return lhs.As<Char>().value == rhs.As<Char>().value;
  } else if (lhs.Is<Number>()) {
    return lhs.As<Number>().value == rhs.As<Number>().value;
  } else if (lhs.Is<String>()) {
    return lhs.As<String>().value == rhs.As<String>().value;
  } else if (lhs.Is<Id>()) {
    return lhs.As<Id>().value == rhs.As<Id>().value;
  } else {
    return true;
  }
}

std::ostream& operator << (std::ostream& os, const Token& rhs) {
  using namespace TokenType;

#define VALUED_OUTPUT(type) \
  if (auto p = rhs.TryAs<type>()) return os << #type << '{' << p->value << '}';

  VALUED_OUTPUT(Number);
  VALUED_OUTPUT(Id);
  VALUED_OUTPUT(String);
  VALUED_OUTPUT(Char);

#undef VALUED_OUTPUT

#define UNVALUED_OUTPUT(type) \
    if (rhs.Is<type>()) return os << #type;

  UNVALUED_OUTPUT(Class);
  UNVALUED_OUTPUT(Return);
  UNVALUED_OUTPUT(If);
  UNVALUED_OUTPUT(Else);
  UNVALUED_OUTPUT(Def);
  UNVALUED_OUTPUT(Newline);
  UNVALUED_OUTPUT(Print);
  UNVALUED_OUTPUT(Indent);
  UNVALUED_OUTPUT(Dedent);
  UNVALUED_OUTPUT(And);
  UNVALUED_OUTPUT(Or);
  UNVALUED_OUTPUT(Not);
  UNVALUED_OUTPUT(Eq);
  UNVALUED_OUTPUT(NotEq);
  UNVALUED_OUTPUT(LessOrEq);
  UNVALUED_OUTPUT(GreaterOrEq);
  UNVALUED_OUTPUT(None);
  UNVALUED_OUTPUT(True);
  UNVALUED_OUTPUT(False);
  UNVALUED_OUTPUT(Eof);

#undef UNVALUED_OUTPUT

  return os << "Unknown token :(";
}

class Lexer::Impl {
  enum State {
    LINE_START,
    COUNT_INDENT,
    IN_LINE,
    LINE_END,
    FILE_END
  };

 public:
  Impl(istream& input);

  Token NextToken();

 private:
  istream& input_;
  State state_ = LINE_START;
  string accumulator_;
  size_t indent_state_ = 0;
  size_t current_indent_ = 0;

  optional<Token> Go(int ch);
  optional<Token> FlushToken(int ch);
};


Lexer::Impl::Impl(istream& input)
  : input_(input)
{}

Token Lexer::Impl::NextToken() {
  optional<Token> v;
  while (!v.has_value()) {
    v = Go(input_.get());
  }
  return move(v.value());
}

optional<Token> Lexer::Impl::Go(int ch) {
  switch (state_) {
    case LINE_START:
      if (accumulator_ == "  ") {
        ++current_indent_;
        accumulator_.clear();
      }

      if (ch == ' ') {
        accumulator_.push_back(ch);
        return {};
      }

      state_ = COUNT_INDENT;
      return Go(ch);

    case COUNT_INDENT:
      if (!accumulator_.empty()) {
        throw LexerError("wrong indent");
      }

      input_.putback(ch);
      if (indent_state_ < current_indent_) {
        ++indent_state_;
        return TokenType::Indent{};
      } else if (indent_state_ > current_indent_) {
        --indent_state_;
        return TokenType::Dedent{};
      }

      current_indent_ = 0;
      state_ = IN_LINE;
      return {};

    case IN_LINE:
      auto v = FlushToken(ch);

      if (ch == EOF) {
        if (v.has_value()) {
          return move(v.value());
        }
        state_ = FILE_END;
        return Go(ch);

      } else if (ch == '\n') {
        if (v.has_value()) {
          return move(v.value());
        }
        state_ = LINE_END;
        return Go(ch);

      } else if (!isspace(ch)) {
        if (v.has_value()) {
          return move(v.value());
        }
      }

    case LINE_END:
      state_ = LINE_START;
      return TokenType::Newline{};

    case FILE_END:
      return TokenType::Eof{};
  }
  throw LexerError("unknown state: " + to_string(state_));
}


Lexer::Lexer(std::istream& input)
  : pimpl_(make_unique<Impl>(input))
{
  NextToken();
}

Lexer::~Lexer() = default;

const Token& Lexer::CurrentToken() const {
  if (current_token_) {
    return current_token_.value();
  }
  throw LexerError("no current token");
}

Token Lexer::NextToken() {
  Token t = pimpl_->NextToken();
  current_token_ = t;
  return t;
}

} /* namespace Parse */
