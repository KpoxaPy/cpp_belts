#pragma once

#include <iosfwd>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>

class TestRunner;

namespace Parse {

namespace TokenType {
  struct Number {
    int value;
  };

  struct Id {
    std::string value;
  };

  struct Char {
    char value;
  };

  struct String {
    std::string value;
  };

  struct Class{};
  struct Return{};
  struct If {};
  struct Else {};
  struct Def {};
  struct Newline {};
  struct Print {};
  struct Indent {};
  struct Dedent {};
  struct Eof {};
  struct And {};
  struct Or {};
  struct Not {};
  struct Eq {};
  struct NotEq {};
  struct LessOrEq {};
  struct GreaterOrEq {};
  struct None {};
  struct True {};
  struct False {};
}

using TokenBase = std::variant<
  TokenType::Number,
  TokenType::Id,
  TokenType::Char,
  TokenType::String,
  TokenType::Class,
  TokenType::Return,
  TokenType::If,
  TokenType::Else,
  TokenType::Def,
  TokenType::Newline,
  TokenType::Print,
  TokenType::Indent,
  TokenType::Dedent,
  TokenType::And,
  TokenType::Or,
  TokenType::Not,
  TokenType::Eq,
  TokenType::NotEq,
  TokenType::LessOrEq,
  TokenType::GreaterOrEq,
  TokenType::None,
  TokenType::True,
  TokenType::False,
  TokenType::Eof
>;

struct Token : TokenBase {
  using TokenBase::TokenBase;

  template <typename T>
  bool Is() const {
    return std::holds_alternative<T>(*this);
  }

  template <typename T>
  const T& As() const {
    return std::get<T>(*this);
  }

  template <typename T>
  const T* TryAs() const {
    return std::get_if<T>(this);
  }
};

bool operator == (const Token& lhs, const Token& rhs);
std::ostream& operator << (std::ostream& os, const Token& rhs);

class LexerError : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

class Lexer {
public:
  explicit Lexer(std::istream& input);
  ~Lexer();

  const Token& CurrentToken() const;
  Token NextToken();

  template <typename T>
  const T& Expect() const {
    if (auto t = CurrentToken().TryAs<T>(); t) {
      return *t;
    }
    throw LexerError("Unexpected current token");
  }

  template <typename T, typename U>
  void Expect(const U& value) const {
    auto t = Expect<T>();
    if (!(t.value == value)) {
      std::ostringstream ss;
      ss << "Unexpected token value \"" << t.value << "\", expected \"" << value << "\"";
      throw LexerError(ss.str());
    }
  }

  template <typename T>
  const T& ExpectNext() {
    NextToken();
    return Expect<T>();
  }

  template <typename T, typename U>
  void ExpectNext(const U& value) {
    NextToken();
    Expect<T, U>(value);
  }

private:
  std::optional<Token> current_token_;

  class Impl;
  std::unique_ptr<Impl> pimpl_;
};

void RunLexerTests(TestRunner& test_runner);

} /* namespace Parse */
