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
    START,
    COUNT_INDENT,
    CHECK_INDENT,
    LINE_START,
    LINE,
    IDENT,
    NUMBER,
    STR_LITERAL,
    SYMBOL_2,
    END_OF_FILE,
    ERROR
  };

 public:
  Impl(istream& input);

  Token NextToken();

 private:
  static const string SYMBOLS_1;
  static const string SYMBOLS_2;

  istream& input_;

  State state_ = START;
  size_t indent_state_ = 0;

  string accumulator_;
  string context_;
  int number_;

  size_t start_line_spaces_ = 0;
  size_t check_indent_current_ = 0;
  State check_indent_next_state_ = START;

  char str_literal_bound_ = 0;

  char symbol_2_first_;

  void Clear();
  void Add(int ch);
  Token FlushIdent();

  int GetChar();
  void PutBack(int ch);

  optional<Token> Go(int ch);
};

const string Lexer::Impl::SYMBOLS_1 = "+-*/().:,?";
const string Lexer::Impl::SYMBOLS_2 = "<>=!";

Lexer::Impl::Impl(istream& input)
  : input_(input)
{}

Token Lexer::Impl::NextToken() {
  optional<Token> v;
  while (!v.has_value()) {
    v = Go(GetChar());
  }
  return move(v.value());
}

void Lexer::Impl::Clear() {
  accumulator_.clear();
}

void Lexer::Impl::Add(int ch) {
  accumulator_.push_back(ch);
}

Token Lexer::Impl::FlushIdent() {
  if (accumulator_ == "class") {
    return TokenType::Class{};
  } else if (accumulator_ == "return") {
    return TokenType::Return{};
  } else if (accumulator_ == "if") {
    return TokenType::If{};
  } else if (accumulator_ == "else") {
    return TokenType::Else{};
  } else if (accumulator_ == "def") {
    return TokenType::Def{};
  } else if (accumulator_ == "print") {
    return TokenType::Print{};
  } else if (accumulator_ == "and") {
    return TokenType::And{};
  } else if (accumulator_ == "or") {
    return TokenType::Or{};
  } else if (accumulator_ == "not") {
    return TokenType::Not{};
  } else if (accumulator_ == "None") {
    return TokenType::None{};
  } else if (accumulator_ == "True") {
    return TokenType::True{};
  } else if (accumulator_ == "False") {
    return TokenType::False{};
  }

  return TokenType::Id{accumulator_};
}

int Lexer::Impl::GetChar() {
  int ch = input_.get();
  if (ch != EOF) {
    context_.push_back(ch);
  }
  return ch;
}

void Lexer::Impl::PutBack(int ch) {
  if (context_.length() > 0) {
    context_.pop_back();
  }
  input_.putback(ch);
}

optional<Token> Lexer::Impl::Go(int ch) {
  switch (state_) {
  case START:
    // context_.clear();
    check_indent_current_ = 0;
    if (ch == ' ') {
      start_line_spaces_ = 1;
      state_ = COUNT_INDENT;
    } else if (ch == EOF) {
      check_indent_next_state_ = END_OF_FILE;
      state_ = CHECK_INDENT;
    } else {
      PutBack(ch);
      state_ = LINE_START;
    }
    return {};

  case COUNT_INDENT:
    if (ch == ' ') {
      ++start_line_spaces_;
      return {};
    }

    PutBack(ch);
    if (start_line_spaces_ != 0) {
      if (start_line_spaces_ % 2 == 1) {
        state_ = ERROR;
        throw LexerError("Wrong indent");
      }
      check_indent_current_ = start_line_spaces_ / 2;
    }

    state_ = LINE_START;
    return {};

  case CHECK_INDENT:
    PutBack(ch);
    if (indent_state_ < check_indent_current_) {
      ++indent_state_;
      return TokenType::Indent{};
    } else if (indent_state_ > check_indent_current_) {
      --indent_state_;
      return TokenType::Dedent{};
    }
    state_ = check_indent_next_state_;
    return {};

  case LINE_START:
    if (ch == '\r' || ch == '\t') {
      return {};
    }

    if (ch == '\n') {
      state_ = START;
      return {};
    }

    PutBack(ch);
    check_indent_next_state_ = LINE;
    state_ = CHECK_INDENT;
    return {};

  case LINE:
    if (ch == '\'' || ch == '"') {
      Clear();
      str_literal_bound_ = ch;
      state_ = STR_LITERAL;
      return {};
    }

    if (SYMBOLS_1.find(ch) != string::npos) {
      return TokenType::Char{static_cast<char>(ch)};
    }

    if (SYMBOLS_2.find(ch) != string::npos) {
      symbol_2_first_ = ch;
      state_ = SYMBOL_2;
      return {};
    }

    if (ch == '_' || ('A' <= ch && ch <= 'z')) {
      Clear();
      Add(ch);
      state_ = IDENT;
      return {};
    }

    if ('0' <= ch && ch <= '9') {
      number_ = ch - '0';
      state_ = NUMBER;
      return {};
    }

    if (ch == ' ' || ch == '\r' || ch == '\t') {
      return {};
    }

    if (ch == '\n') {
      state_ = START;
      return TokenType::Newline{};
    }

    if (ch == EOF) {
      check_indent_current_ = 0;
      check_indent_next_state_ = END_OF_FILE;
      state_ = CHECK_INDENT;
      return TokenType::Newline{};
    }

    state_ = ERROR;
    {
      std::ostringstream ss;
      ss << "Unknown symbol in LINE state: " << static_cast<char>(ch) << "(" << ch << ")"
        << " context=\"" << context_ << "\"";
      throw LexerError(ss.str());
    }

  case IDENT:
    if (ch == '_'
      || ('A' <= ch && ch <= 'z')
      || ('0' <= ch && ch <= '9'))
    {
      Add(ch);
      return {};
    }
    PutBack(ch);
    state_ = LINE;
    return FlushIdent();

  case NUMBER:
    if ('0' <= ch && ch <= '9') {
      number_ = number_ * 10 + (ch - '0');
      return {};
    }
    PutBack(ch);
    state_ = LINE;
    return TokenType::Number{number_};

  case STR_LITERAL:
    if (ch == EOF || ch == '\n') {
      state_ = ERROR;
      throw LexerError("Unexpected end of string literal");
    }

    if (ch != str_literal_bound_) {
      Add(ch);
      return {};
    }

    state_ = LINE;
    return TokenType::String{accumulator_};

  case SYMBOL_2:
    if (ch != '=') {
      if (symbol_2_first_ == '!') {
        state_ = ERROR;
        std::ostringstream ss;
        ss << "Expected !=, but got !" << static_cast<char>(ch)
          << " context=\"" << context_ << "\"";
        throw LexerError(ss.str());
      }
      PutBack(ch);
      state_ = LINE;
      return TokenType::Char{symbol_2_first_};
    }

    state_ = LINE;
    if (symbol_2_first_ == '!') {
      return TokenType::NotEq{};
    } else if (symbol_2_first_ == '=') {
      return TokenType::Eq{};
    } else if (symbol_2_first_ == '<') {
      return TokenType::LessOrEq{};
    } else if (symbol_2_first_ == '>') {
      return TokenType::GreaterOrEq{};
    }

    state_ = ERROR;
    {
      std::ostringstream ss;
      ss << "Unexpected first symbol in two symbol operator: "
        << symbol_2_first_ << static_cast<char>(ch)
        << " context=\"" << context_ << "\"";
      throw LexerError(ss.str());
    }

  case END_OF_FILE:
    return TokenType::Eof{};

  case ERROR:
    throw LexerError("lexer in error state");

  default:
    state_ = ERROR;
    throw LexerError("unknown state: " + to_string(state_));
  }
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
