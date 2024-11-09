#include "Expr.hpp"
#include "Function.hpp"
#include "Interpreter.hpp"
#include "Utils.hpp"

#include <absl/base/macros.h>
#include <absl/container/inlined_vector.h>
#include <absl/strings/string_view.h>

#include <fmt/format.h>

#include <cstddef>
#include <memory>
#include <optional>

namespace lox {

ExprResult Interpreter::evaluate(ExprPtr expr) { return expr->accept(*this); }

bool isEqual(ExprResult l, ExprResult r) {
  auto visitor = util::Overloaded(
      [](bool a, bool b) { return a == b; },
      [](double a, double b) { return a == b; },
      [](std::string a, std::string b) { return a == b; },
      [](std::nullptr_t, std::nullptr_t) { return true; },
      [](auto, auto) { return false; });
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
  for (auto &e : util::make_reverse(envs_)) {
    if (e.assign(a.name_, val)) return val;
  }
  auto resolved = global_.assign(a.name_, val);
  ABSL_ASSERT(resolved &&
      "Variable being assigned to not found in any environment");
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
        util::Overloaded{
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
  default: util::unreachable();
  }
}

ExprResult Interpreter::visitCallExpr(Call &expr) {
  auto res = evaluate(expr.callee_);
  Args args;
  for (auto &arg : expr.args_) args.push_back(evaluate(arg));
  auto evaluate_call_expr = [this, &args](CallablePtr &func) -> ExprResult {
    if (args.size() != func->arity())
      throw RuntimeError(
          fmt::format("Expected {} arguments to function, got {}.",
                      func->arity(), args.size()));
    return (*func)(*this, std::move(args));
  };
  auto error_case = [](auto) -> ExprResult {
    throw RuntimeError("Attempted to call expression that was not a function");
    return {};
  };

  return std::visit(util::Overloaded{evaluate_call_expr, error_case}, res);
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
  default:
    util::unreachable();
    // TODO: test this. Can it be reached?
  }
}

ExprResult Interpreter::visitVariableExpr(Variable &v) {
  auto name = v.name_;
  for (auto &e : util::make_reverse(envs_)) {
    auto res = e.get(name);
    if (res) return *res;
  }
  auto res = global_.get(name);
  if (res) return *res;

  throw RuntimeError(absl::StrCat("Undefined variable: ", name.identifier()));
}

void Interpreter::visitBlockStmt(Block &b) { executeBlock(b.statements_); }

void Interpreter::visitExpressionStmt(Expression &e) {
  evaluate(e.expression_);
}

void Interpreter::visitFnStmt(Fn &f) {
  auto func = std::make_shared<Function>(f);
  current().define(f.name_.lexeme(), func);
}

void Interpreter::visitIfStmt(If &i) {
  if (isTruthy(evaluate(i.condition_))) {
    execute(*i.then_);
  } else if (i.else_br_) {
    execute(*i.else_br_);
  }
}

void Interpreter::visitVarStmt(Var &v) {
  current().define(v.name_.identifier(),
                   v.initialiser_ ? evaluate(v.initialiser_) : nullptr);
}

void Interpreter::visitWhileStmt(While &w) {
  while (isTruthy(evaluate(w.condition_))) { execute(*w.body_); }
}

// TODO: fix the catch
void Interpreter::executeBlock(const StatementsList &stmts,
                               std::optional<Environment> &&env) {
  std::optional<EnvironmentStack> prior;
  if (env) {
    prior = std::move(envs_);
    envs_ = EnvironmentStack{};
  }
  envs_.push_back(env.value_or(Environment{}));
  try {
    for (auto &stmt : stmts) {
      ABSL_ASSERT(stmt);
      execute(*stmt);
    }
  } catch (...) { ; }
  envs_.pop_back();
  if (prior) { envs_ = std::move(*prior); }
}

void Interpreter::interpret(StatementsList &&list) {
  try {
    for (auto const &stmt : list) { execute(*stmt); }
  } catch (RuntimeError const &e) { report_error(e.what(), Location{}); }
}

} // namespace lox
