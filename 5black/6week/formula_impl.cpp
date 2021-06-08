#include "formula_impl.h"

#include <sstream>
#include <queue>

using namespace std;

namespace {

pair<Position, IFormula::HandlingResult> HandlePosInsertedRows(Position pos, int before, int count) {
  if (pos.row < before) {
    return {pos, IFormula::HandlingResult::NothingChanged};
  }

  pos.row += count;
  return {pos, IFormula::HandlingResult::ReferencesRenamedOnly};
}

pair<Position, IFormula::HandlingResult> HandlePosInsertedCols(Position pos, int before, int count) {
  if (pos.col < before) {
    return {pos, IFormula::HandlingResult::NothingChanged};
  }

  pos.col += count;
  return {pos, IFormula::HandlingResult::ReferencesRenamedOnly};
}

pair<Position, IFormula::HandlingResult> HandlePosDeletedRows(Position pos, int first, int count) {
  if (pos.row < first) {
    return {pos, IFormula::HandlingResult::NothingChanged};
  }

  if (pos.row < first + count) {
    return {pos, IFormula::HandlingResult::ReferencesChanged};
  }

  pos.row -= count;
  return {pos, IFormula::HandlingResult::ReferencesRenamedOnly};
}

pair<Position, IFormula::HandlingResult> HandlePosDeletedCols(Position pos, int first, int count) {
  if (pos.col < first) {
    return {pos, IFormula::HandlingResult::NothingChanged};
  }

  if (pos.col < first + count) {
    return {pos, IFormula::HandlingResult::ReferencesChanged};
  }

  pos.col -= count;
  return {pos, IFormula::HandlingResult::ReferencesRenamedOnly};
}

}

void Formula::SetReferencedCells(vector<Position> refs) {
  referenced_cells_ = std::move(refs);
}

void Formula::SetRootNode(FormulaNode root) {
  root_ = std::move(root);
}

IFormula::Value Formula::Evaluate(const ISheet& sheet) const {
  return EvaluateNode(root_, sheet);
}

std::string Formula::GetExpression() const {
  ostringstream ss;
  ss << root_;
  return ss.str();
}

std::vector<Position> Formula::GetReferencedCells() const {
  return referenced_cells_;
}

IFormula::HandlingResult Formula::HandleInsertedRows(int before, int count) {
  return TraverseChangedCells([before, count](Position pos){
    return HandlePosInsertedRows(pos, before, count);
  });
}

IFormula::HandlingResult Formula::HandleInsertedCols(int before, int count) {
  return TraverseChangedCells([before, count](Position pos){
    return HandlePosInsertedCols(pos, before, count);
  });
}

IFormula::HandlingResult Formula::HandleDeletedRows(int first, int count) {
  return TraverseChangedCells([first, count](Position pos){
    return HandlePosDeletedRows(pos, first, count);
  });
}

IFormula::HandlingResult Formula::HandleDeletedCols(int first, int count) {
  return TraverseChangedCells([first, count](Position pos){
    return HandlePosDeletedCols(pos, first, count);
  });
}

IFormula::HandlingResult Formula::TraverseChangedCells(CellTraverser f) {
  auto res = TraverseChangedCellsForRefs(f);
  TraverseChangedCellsForFormula(f);
  return res;
}

IFormula::HandlingResult Formula::TraverseChangedCellsForRefs(CellTraverser f) {
  vector<Position> new_refs;
  HandlingResult h_result = HandlingResult::NothingChanged;

  for (auto pos : referenced_cells_) {
    auto [new_pos, res] = f(pos);
    if (res != HandlingResult::ReferencesChanged) {
      new_refs.push_back(new_pos);
    }
    if (res > h_result) {
      h_result = res;
    }
  }

  referenced_cells_ = std::move(new_refs);
  return h_result;
}

void Formula::TraverseChangedCellsForFormula(CellTraverser f) {
  queue<FormulaNode*> q;
  q.push(&root_);

  while (!q.empty()) {
    auto node = q.front();
    q.pop();

    if (!node) {
      continue;
    }

    if (holds_alternative<Position>(*node)) {
      auto [new_pos, res] = f(get<Position>(*node));
      if (res == HandlingResult::ReferencesRenamedOnly) {
        *node = new_pos;
      } else if (res == HandlingResult::ReferencesChanged) {
        *node = FormulaError(FormulaError::Category::Ref);
      }
    } else if (holds_alternative<BinaryOp>(*node)) {
      auto& op = get<BinaryOp>(*node);
      q.push(op.lhs.get());
      q.push(op.rhs.get());
    } else if (holds_alternative<UnaryOp>(*node)) {
      q.push(get<UnaryOp>(*node).arg.get());
    }
  }
}