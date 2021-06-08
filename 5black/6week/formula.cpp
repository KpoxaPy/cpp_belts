#include "formula.h"

#include <cmath>
#include <list>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <variant>
#include <vector>

#include "FormulaLexer.h"
#include "FormulaListener.h"
#include "FormulaParser.h"
#include "antlr4-runtime.h"
#include "exception.h"
#include "formula_node_full.h"

using namespace std;

class Formula : public IFormula {
 public:
  void SetReferencedCells(vector<Position> refs) {
    referenced_cells_ = std::move(refs);
  }

  void SetRootNode(FormulaNode root) {
    root_ = std::move(root);
  }

  Value Evaluate(const ISheet& sheet) const override {
    return EvaluateNode(root_, sheet);
  }

  std::string GetExpression() const override {
    ostringstream ss;
    ss << root_;
    return ss.str();
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
  vector<Position> referenced_cells_;
  FormulaNode root_;
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
  void visitTerminal(antlr4::tree::TerminalNode *node) override {}
  void visitErrorNode(antlr4::tree::ErrorNode *node) override {}
  void enterEveryRule(antlr4::ParserRuleContext *ctx) override {}
  void exitEveryRule(antlr4::ParserRuleContext *ctx) override {}

  void enterMain(FormulaParser::MainContext *ctx) override {}
  void exitMain(FormulaParser::MainContext *ctx) override {}

  void enterUnaryOp(FormulaParser::UnaryOpContext *ctx) override {}
  void exitUnaryOp(FormulaParser::UnaryOpContext *ctx) override {
    auto arg = std::move(memory_.top());
    memory_.pop();
    if (ctx->ADD()) {
      memory_.push(UnaryOp{'+', make_unique<FormulaNode>(std::move(arg))});
    } else if (ctx->SUB()) {
      memory_.push(UnaryOp{'-', make_unique<FormulaNode>(std::move(arg))});
    }
  }

  void enterParens(FormulaParser::ParensContext *ctx) override {}
  void exitParens(FormulaParser::ParensContext *ctx) override {}

  void enterLiteral(FormulaParser::LiteralContext *ctx) override {}
  void exitLiteral(FormulaParser::LiteralContext *ctx) override {
    memory_.push(stod(ctx->NUMBER()->getText()));
  }

  void enterCell(FormulaParser::CellContext *ctx) override {}
  void exitCell(FormulaParser::CellContext *ctx) override {
    auto pos = Position::FromString(ctx->CELL()->getText());
    if (!pos.IsValid()) {
      throw FormulaException("Wrong cell ref");
    }
    refs_.insert(pos);
    memory_.push(pos);
  }

  void enterBinaryOp(FormulaParser::BinaryOpContext *ctx) override {}
  void exitBinaryOp(FormulaParser::BinaryOpContext *ctx) override {
    auto rhs = std::move(memory_.top());
    memory_.pop();
    auto lhs = std::move(memory_.top());
    memory_.pop();

    if (ctx->MUL()) {
      memory_.push(BinaryOp{'*',
        make_unique<FormulaNode>(std::move(lhs)),
        make_unique<FormulaNode>(std::move(rhs))});
    } else if (ctx->DIV()) {
      memory_.push(BinaryOp{'/',
        make_unique<FormulaNode>(std::move(lhs)),
        make_unique<FormulaNode>(std::move(rhs))});
    } else if (ctx->ADD()) {
      memory_.push(BinaryOp{'+',
        make_unique<FormulaNode>(std::move(lhs)),
        make_unique<FormulaNode>(std::move(rhs))});
    } else if (ctx->SUB()) {
      memory_.push(BinaryOp{'-',
        make_unique<FormulaNode>(std::move(lhs)),
        make_unique<FormulaNode>(std::move(rhs))});
    }
  }

  unique_ptr<IFormula> FlushResult() {
    auto formula = make_unique<Formula>();

    vector<Position> refs(refs_.begin(), refs_.end());
    formula->SetReferencedCells(std::move(refs));
    formula->SetRootNode(std::move(memory_.top()));

    return formula;
  }

 private:
  set<Position> refs_;
  stack<FormulaNode> memory_;
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
