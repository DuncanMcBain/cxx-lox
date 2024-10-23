#include "Interpreter.h"
#include "Expr.h"
#include "Utils.h"

#include <absl/base/macros.h>
#include <absl/container/inlined_vector.h>
#include <absl/strings/string_view.h>

#include <fmt/format.h>

#include <cstddef>
#include <memory>

template <typename... Ts>
struct Overloaded : public Ts... {
  using Ts::operator()...;
};
template <typename... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

namespace lox {

ExprResult Interpreter::evaluate(std::shared_ptr<Expr> expr) {
  return expr->accept(*this);
}

bool isEqual(ExprResult l, ExprResult r) {
  auto visitor = Overloaded{[](bool a, bool b) { return a == b; },
                            [](double a, double b) { return a == b; },
                            [](std::string a, std::string b) { return a == b; },
                            [](std::nullptr_t, std::nullptr_t) { return true; },
                            [](auto, auto) { return false; }};
  return std::visit(visitor, l, r);
}

bool isTruthy(ExprResult obj) {
  // TODO: can I make 0 false? Maybe some int type later
  struct TruthTeller {
    bool operator()(bool b) { return b; }
    bool operator()(ExprResult) { return false; }
  } tt;
  return std::visit(tt, obj);
}

ExprResult Interpreter::visitAssignExpr(Assign &a) {
  auto val = evaluate(a.val_);
  for (auto &e : make_reverse(envs_)) {
    if (e.assign(a.name_, val)) break;
  }
  return val;
}

// TODO: make the failed std::get exceptions into custom exception that
// will track the token that caused the failure and report nicely to the user
// This function should swallow the exceptions from either std::get or from
// me throwing them directly, then do check the operator and types to provide
// best-possible error messages (throw BinaryOpError?)
ExprResult Interpreter::visitBinaryExpr(Binary &b) {
  auto left  = evaluate(b.left_);
  auto right = evaluate(b.right_);

  using enum TokenType;
  switch (b.op_.type()) {
  case COMMA: return right;
  case MINUS: return std::get<double>(left) - std::get<double>(right);
  case PLUS:
    return std::visit(
        Overloaded{
            [](double a, double b) -> ExprResult { return a + b; },
            [](std::string a, std::string b) -> ExprResult { return a + b; },
            [](auto, auto) -> ExprResult {
              throw RuntimeError("bad args to +");
            }},
        left, right);
  case SLASH: return std::get<double>(left) / std::get<double>(right);
  case STAR: return std::get<double>(left) * std::get<double>(right);
  case GTR: return std::get<double>(left) > std::get<double>(right);
  case GTR_EQ: return std::get<double>(left) >= std::get<double>(right);
  case LESS: return std::get<double>(left) < std::get<double>(right);
  case LESS_EQ: return std::get<double>(left) <= std::get<double>(right);
  case BANG_EQ: return !isEqual(left, right);
  case EQ_EQ: return isEqual(left, right);
  default: unreachable();
  }
}

ExprResult Interpreter::visitLogicalExpr(Logical &l) {
  auto left = evaluate(l.left_);
  if (l.op_.type() == TokenType::OR) {
    if (isTruthy(left)) return left;
  } else {
    if (!isTruthy(left)) return left;
  }
  return evaluate(l.right_);
}

ExprResult Interpreter::visitTernaryExpr(Ternary &t) {
  auto cond = isTruthy(evaluate(t.cond_));
  return cond ? evaluate(t.left_) : evaluate(t.right_);
}

ExprResult Interpreter::visitUnaryExpr(Unary &u) {
  auto right = evaluate(u.right_);
  switch (u.op_.type()) {
  case TokenType::MINUS: return -std::get<double>(right);
  case TokenType::BANG: return !isTruthy(right);
  default: unreachable();
  }
}

ExprResult Interpreter::visitVariableExpr(Variable &v) {
  for (auto &e : make_reverse(envs_)) {
    auto res = e.get(v.name_);
    if (res) return *res;
  }
  throw RuntimeError(
      absl::StrCat("Undefined variable: ", v.name_.identifier()));
}

void Interpreter::visitBlockStmt(Block &b) { executeBlock(b.statements_); }

// TODO: remove the forced print when the print-as-function is implemented
void Interpreter::visitExpressionStmt(Expression &e) {
  fmt::print("{}\n", to_string(evaluate(e.expression_)));
}

void Interpreter::visitIfStmt(If &i) {
  if (isTruthy(evaluate(i.condition_))) {
    execute(*i.then_);
  } else if (i.else_br_) {
    execute(*i.else_br_);
  }
}

void Interpreter::visitVarStmt(Var &v) {
  cur().define(v.name_.identifier(),
               v.initialiser_ ? evaluate(v.initialiser_) : nullptr);
}

}

// TODO: fix the catch
void Interpreter::executeBlock(StatementsList &stmts) {
  envs_.emplace_back();
  try {
    for (auto &stmt : stmts) {
      ABSL_ASSERT(stmt);
      execute(*stmt);
    }
  } catch (...) { ; }
  envs_.pop_back();
}

void Interpreter::interpret(StatementsList &&list) {
  try {
    for (auto const &stmt : list) { execute(*stmt); }
  } catch (RuntimeError const &e) { report_error(e.what(), Location{}); }
}

// TODO: move to Expr.h as inline function maybe? Needs to live somewhere
// better than here
std::string Interpreter::to_string(ExprResult res) {
  auto visitor =
      Overloaded{[](bool a) -> std::string { return a ? "true" : "false"; },
                 [](double a) { return std::to_string(a); },
                 [](std::string a) { return a; },
                 [](std::nullptr_t) -> std::string { return "nil"; }};
  return std::visit(visitor, res);
}

} // namespace lox
