#include <cmath>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>

#include "FormulaLexer.h"
#include "FormulaBaseListener.h"
#include "FormulaParser.h"
#include "antlr4-runtime.h"
#include "common_etc.h"
#include "formula_impl.h"
#include "formula_node_full.h"
#include "profile.h"
#include "sheet_impl.h"

using namespace std;

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

class MyFormulaListener : public FormulaBaseListener {
 public:
  MyFormulaListener(Sheet& sheet)
    : sheet_(sheet)
  {}

  void exitLiteral(FormulaParser::LiteralContext *ctx) override {
    METER_DURATION(sheet_.m_fp_walk_lit);
    memory_.push(stod(ctx->NUMBER()->getSymbol()->getText()));
  }

  void exitCell(FormulaParser::CellContext *ctx) override {
    METER_DURATION(sheet_.m_fp_walk_cell);
    auto pos = Position::FromString(ctx->CELL()->getSymbol()->getText());

    if (!pos.IsValid()) {
      throw FormulaException("Wrong cell ref");
    }

    refs_.insert(pos);
    memory_.push(pos);
  }

  void exitUnaryOp(FormulaParser::UnaryOpContext *ctx) override {
    METER_DURATION(sheet_.m_fp_walk_op_1);
    auto arg = std::move(memory_.top());
    memory_.pop();
    if (ctx->ADD()) {
      memory_.push(UnaryOp{'+', make_unique<FormulaNode>(std::move(arg))});
    } else if (ctx->SUB()) {
      memory_.push(UnaryOp{'-', make_unique<FormulaNode>(std::move(arg))});
    }
  }

  void exitBinaryOp(FormulaParser::BinaryOpContext *ctx) override {
    METER_DURATION(sheet_.m_fp_walk_op_2);
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
    METER_DURATION(sheet_.m_fp_walk_flush);
    auto formula = make_unique<Formula>();

    vector<Position> refs(refs_.begin(), refs_.end());
    formula->SetReferencedCells(std::move(refs));
    formula->SetRootNode(std::move(memory_.top()));
    return formula;
  }

 private:
  Sheet &sheet_;
  set<Position> refs_;
  stack<FormulaNode> memory_;
};

std::unique_ptr<IFormula> ParseFormula(std::string expression, Sheet& sheet) {
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
  {
    METER_DURATION(sheet.m_fp_get_tree);
    try {
      tree = parser.main();
    } catch (std::exception &e) {
      ostringstream ess;
      ess << "Error when parsing: " << e.what();
      throw FormulaException(ess.str());
    }
  }

  MyFormulaListener listener(sheet);
  {
    METER_DURATION(sheet.m_fp_walk);
    antlr4::tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);
  }

  return listener.FlushResult();
}

unique_ptr<IFormula> ParseFormula(string expression) {
  auto sheet = CreateSheet();
  return ParseFormula(std::move(expression), *dynamic_cast<Sheet*>(sheet.get()));
}
