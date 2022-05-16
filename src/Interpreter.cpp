#include "Interpreter.h"
#include "Expr.h"

#include <fmt/format.h>

#include <cstddef>
#include <memory>
#include <exception>

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
  auto visitor = Overloaded{
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

// TODO: make the failed std::get exceptions into custom exception that
// will track the token that caused the failure and report nicely to the user
ExprResult Interpreter::visitBinaryExpr(Binary &b) {
  auto left  = evaluate(b.left_);
  auto right = evaluate(b.right_);

  using enum TokenType;
  switch (b.op_.type()) {
  case COMMA: return right;
  case MINUS: return std::get<double>(left) - std::get<double>(right);
  case PLUS:
    return std::visit(Overloaded{
        [](double a, double b) -> ExprResult { return a + b; },
        [](std::string a, std::string b) -> ExprResult { return a + b; },
        [](auto, auto) -> ExprResult { throw RuntimeError("bad args to +"); }}, left, right);
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

ExprResult Interpreter::visitUnaryExpr(Unary &u) {
  auto right = evaluate(u.right_);
  switch (u.op_.type()) {
  case TokenType::MINUS: return -std::get<double>(right);
  case TokenType::BANG: return !isTruthy(right);
  default: unreachable();
  }
}

void Interpreter::interpret(std::shared_ptr<Expr> expr) {
  try {
    auto val = evaluate(expr);
    fmt::print("{}\n", to_string(val));
  } catch(const std::exception& e) {
    fmt::print("{}", e.what());
  }
}

// TODO: move to Expr.h as inline function maybe? Needs to live somewhere
// better than here
std::string Interpreter::to_string(ExprResult res) {
  auto visitor = Overloaded{
    [](bool a) -> std::string { return a ? "true" : "false"; },
    [](double a) { return std::to_string(a); },
    [](std::string a) { return a; },
    [](std::nullptr_t) -> std::string { return "nil"; }
  };
  return std::visit(visitor, res);
}

} // namespace lox
