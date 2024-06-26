#ifndef LOX_INTERPRETER_H_
#define LOX_INTERPRETER_H_

#include "Environment.h"
#include "Expr.h"
#include "Stmt.h"

#include <absl/container/inlined_vector.h>
#include <absl/strings/string_view.h>

#include <memory>

namespace lox {

class Interpreter
    : public expr::Visitor
    , stmt::Visitor {
  Environment env_;

  ExprResult evaluate(std::shared_ptr<Expr>);
  void execute(Stmt &stmt) { stmt.accept(*this); }
  void executeBlock(StatementsList&, Environment&);
  std::string to_string(ExprResult);

 public:
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

  void interpret(StatementsList &&);
};

} // namespace lox

#endif // LOX_INTERPRETER_H_
