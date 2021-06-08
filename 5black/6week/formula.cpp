#include "formula.h"

#include <list>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <variant>
#include <vector>
#include <cmath>

#include "FormulaLexer.h"
#include "FormulaListener.h"
#include "FormulaParser.h"
#include "antlr4-runtime.h"
#include "exception.h"

using namespace std;

struct BinaryOp {
  char op;
  IFormula::Value operator()(double lhs, double rhs) const {
    if (op == '*') {
      return lhs * rhs;
    } else if (op == '/') {
      if (!isfinite(rhs)) {
        return FormulaError(FormulaError::Category::Div0);
      } else {
        return lhs / rhs;
      }
    } else if (op == '+') {
      return lhs + rhs;
    } else if (op == '-') {
      return lhs - rhs;
    }
    throw runtime_error("unknown op");
  }
};

struct UnaryOp {
  char op;
  IFormula::Value operator()(double arg) const {
    if (op == '+') {
      return arg;
    } else if (op == '-') {
      return - arg;
    }
    throw runtime_error("unknown op");
  }
};

using FormulaNode = variant<double, BinaryOp, UnaryOp, Position>;

class Formula : public IFormula {
 public:
  void AddNode(FormulaNode node) {
    poliz_.push_back(std::move(node));
  }

  void SetReferencedCells(vector<Position> refs) {
    referenced_cells_ = std::move(refs);
  }

  Value Evaluate(const ISheet& sheet) const override {
    stack<double> memory;
    for (auto& e : poliz_) {
      if (holds_alternative<double>(e)) {
        memory.push(get<double>(e));
      } else if (holds_alternative<Position>(e)) {
        auto pos = get<Position>(e);
        auto cell_ptr = sheet.GetCell(pos);
        if (!cell_ptr) {
          memory.push(0.0);
        } else {
          auto value = cell_ptr->GetValue();
          if (holds_alternative<double>(value)) {
            memory.push(get<double>(value));
          } else if (holds_alternative<string>(value)) {
            if (get<string>(value).empty()) {
              memory.push(0.0);
            } else {
              return FormulaError(FormulaError::Category::Value);
            }
          } else if (holds_alternative<FormulaError>(value)) {
            return get<FormulaError>(value);
          }
        }
        throw runtime_error("undecidable cell value");
      } else if (holds_alternative<BinaryOp>(e)) {
        auto rhs = memory.top();
        memory.pop();
        auto lhs = memory.top();
        memory.pop();
        auto result = get<BinaryOp>(e)(lhs, rhs);

        if (holds_alternative<double>(result)) {
          memory.push(get<double>(result));
        } else {
          return result;
        }
      } else if (holds_alternative<UnaryOp>(e)) {
        auto arg = memory.top();
        memory.pop();
        auto result = get<UnaryOp>(e)(arg);

        if (holds_alternative<double>(result)) {
          memory.push(get<double>(result));
        } else {
          return result;
        }
      }
    }
    return memory.top();
  }

  std::string GetExpression() const override {
    return "=";
  }

  std::vector<Position> GetReferencedCells() const override {
    return referenced_cells_;
  }

  HandlingResult HandleInsertedRows(int before, int count) override {
    return HandlingResult::NothingChanged;
  }

  HandlingResult HandleInsertedCols(int before, int count) override {
    return HandlingResult::NothingChanged;
  }

  HandlingResult HandleDeletedRows(int first, int count) override {
    return HandlingResult::NothingChanged;
  }

  HandlingResult HandleDeletedCols(int first, int count) override {
    return HandlingResult::NothingChanged;
  }

 private:
  list<FormulaNode> poliz_;
  vector<Position> referenced_cells_;
};

class BailErrorListener : public antlr4::BaseErrorListener {
 public:
  void syntaxError(antlr4::Recognizer* /* recognizer */,
                   antlr4::Token* /* offendingSymbol */, size_t /* line */,
                   size_t /* charPositionInLine */, const std::string& msg,
                   std::exception_ptr /* e */
                   ) override {
    throw FormulaException("Error when lexing: " + msg);
  }
};

class MyFormulaListener : public FormulaListener {
 public:
  MyFormulaListener() {
    formula_ = make_unique<Formula>();
  }
  void visitTerminal(antlr4::tree::TerminalNode *node) override {}
  void visitErrorNode(antlr4::tree::ErrorNode *node) override {}
  void enterEveryRule(antlr4::ParserRuleContext *ctx) override {}
  void exitEveryRule(antlr4::ParserRuleContext *ctx) override {}

  void enterMain(FormulaParser::MainContext *ctx) override {}
  void exitMain(FormulaParser::MainContext *ctx) override {}

  void enterUnaryOp(FormulaParser::UnaryOpContext *ctx) override {}
  void exitUnaryOp(FormulaParser::UnaryOpContext *ctx) override {
    if (ctx->ADD()) {
      formula_->AddNode(UnaryOp{'+'});
    } else if (ctx->SUB()) {
      formula_->AddNode(UnaryOp{'-'});
    }
  }

  void enterParens(FormulaParser::ParensContext *ctx) override {}
  void exitParens(FormulaParser::ParensContext *ctx) override {}

  void enterLiteral(FormulaParser::LiteralContext *ctx) override {}
  void exitLiteral(FormulaParser::LiteralContext *ctx) override {
    formula_->AddNode(stod(ctx->NUMBER()->getText()));
  }

  void enterCell(FormulaParser::CellContext *ctx) override {}
  void exitCell(FormulaParser::CellContext *ctx) override {
    auto pos = Position::FromString(ctx->CELL()->getText());
    if (!pos.IsValid()) {
      throw FormulaException("Wrong cell ref");
    }
    refs_.insert(pos);
    formula_->AddNode(pos);
  }

  void enterBinaryOp(FormulaParser::BinaryOpContext *ctx) override {}
  void exitBinaryOp(FormulaParser::BinaryOpContext *ctx) override {
    if (ctx->MUL()) {
      formula_->AddNode(BinaryOp{'*'});
    } else if (ctx->DIV()) {
      formula_->AddNode(BinaryOp{'/'});
    } else if (ctx->ADD()) {
      formula_->AddNode(BinaryOp{'+'});
    } else if (ctx->SUB()) {
      formula_->AddNode(BinaryOp{'-'});
    }
  }

  unique_ptr<IFormula> FlushResult() {
    vector<Position> refs(refs_.begin(), refs_.end());
    formula_->SetReferencedCells(std::move(refs));
    return std::move(formula_);
  }

 private:
  set<Position> refs_;
  unique_ptr<Formula> formula_;
};

unique_ptr<IFormula> ParseFormula(string expression) {
  istringstream ss(expression);
  antlr4::ANTLRInputStream input(ss);

  FormulaLexer lexer(&input);
  BailErrorListener error_listener;
  lexer.removeErrorListeners();
  lexer.addErrorListener(&error_listener);

  antlr4::CommonTokenStream tokens(&lexer);

  FormulaParser parser(&tokens);
  auto error_handler = std::make_shared<antlr4::BailErrorStrategy>();
  parser.setErrorHandler(error_handler);
  parser.removeErrorListeners();

  antlr4::tree::ParseTree* tree = nullptr;
  try {
    tree = parser.main();
  } catch (std::exception& e) {
    ostringstream ess;
    ess << "Error when parsing: " << e.what();
    throw FormulaException(ess.str());
  }

  MyFormulaListener listener;
  antlr4::tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);

  return listener.FlushResult();
}
