#ifndef LOX_INTERPRETER_HPP
#define LOX_INTERPRETER_HPP

#include "Builtins.hpp"
#include "Environment.hpp"
#include "Expr.hpp"
#include "Stmt.hpp"

#include <absl/container/inlined_vector.h>

#include <memory>
#include <optional>

namespace lox {

class Interpreter
    : public expr::Visitor<ExprResult>
    , stmt::Visitor<void> {
  Environment global_;
  EnvironmentStack envs_;

  Environment &current() { return envs_.size() ? *(envs_.end() - 1) : global_; }

  ExprResult evaluate(ExprPtr);
  void execute(Stmt &stmt) { stmt.accept(*this); }

  ExprResult visitBoolLiteralExpr(BoolLiteral &b) override { return b.value_; }
  ExprResult visitNullLiteralExpr(NullLiteral &n) override { return nullptr; }
  ExprResult visitNumLiteralExpr(NumLiteral &l) override { return l.value_; }
  ExprResult visitStrLiteralExpr(StrLiteral &s) override { return s.value_; }
  ExprResult visitGroupExpr(Group &g) override { return evaluate(g.expr_); }
  ExprResult visitAssignExpr(Assign &) override;
  ExprResult visitBinaryExpr(Binary &) override;
  ExprResult visitCallExpr(Call &) override;
  ExprResult visitLogicalExpr(Logical &) override;
  ExprResult visitTernaryExpr(Ternary &) override;
  ExprResult visitUnaryExpr(Unary &) override;
  ExprResult visitVariableExpr(Variable &) override;

  void visitBlockStmt(Block &) override;
  void visitExpressionStmt(Expression &) override;
  void visitFnStmt(Fn &) override;
  void visitIfStmt(If &) override;
  void visitVarStmt(Var &) override;
  void visitWhileStmt(While &) override;

 public:
  Interpreter() {
    current().define("now", std::shared_ptr<Callable>(new Now{}));
    current().define("print", std::shared_ptr<Callable>(new Print{}));
  }

  void interpret(StatementsList &&);
  void executeBlock(const StatementsList &,
                    std::optional<Environment> && = std::nullopt);
};

} // namespace lox

#endif // LOX_INTERPRETER_HPP
