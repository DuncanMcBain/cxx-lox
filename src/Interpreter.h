#ifndef LOX_INTERPRETER_H_
#define LOX_INTERPRETER_H_

#include "Environment.h"
#include "Expr.h"
#include "Stmt.h"

#include <absl/container/inlined_vector.h>

#include <memory>

namespace lox {

class Interpreter
    : public expr::Visitor<ExprResult>
    , stmt::Visitor<void> {
  using EnvironmentStack = absl::InlinedVector<Environment, 8>;
  EnvironmentStack envs_ = EnvironmentStack{{}};

  Environment& cur() {
    ABSL_ASSERT(envs_.size() > 0);
    return *(envs_.end() - 1);
  }

  ExprResult evaluate(std::shared_ptr<Expr>);
  void execute(Stmt &stmt) { stmt.accept(*this); }
  void executeBlock(StatementsList&);
  std::string to_string(ExprResult);

  ExprResult visitBoolLiteralExpr(BoolLiteral &b) override { return b.value_; }
  ExprResult visitNumLiteralExpr(NumLiteral &l) override { return l.value_; }
  ExprResult visitStrLiteralExpr(StrLiteral &s) override { return s.value_; }
  ExprResult visitNullLiteralExpr(NullLiteral &n) override { return nullptr; }
  ExprResult visitGroupExpr(Group &g) override { return evaluate(g.expr_); }
  ExprResult visitAssignExpr(Assign &) override;
  ExprResult visitBinaryExpr(Binary &) override;
  ExprResult visitLogicalExpr(Logical &) override;
  ExprResult visitTernaryExpr(Ternary &) override;
  ExprResult visitUnaryExpr(Unary &) override;
  ExprResult visitVariableExpr(Variable &) override;

  void visitBlockStmt(Block &) override;
  void visitExpressionStmt(Expression &) override;
  void visitIfStmt(If &) override;
  void visitVarStmt(Var &) override;
  void visitWhileStmt(While &) override;

 public:
  void interpret(StatementsList &&);
};

} // namespace lox

#endif // LOX_INTERPRETER_H_
