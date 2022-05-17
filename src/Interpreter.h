#ifndef LOX_INTERPRETER_H_
#define LOX_INTERPRETER_H_

#include "Expr.h"
#include "Stmt.h"
#include "Utils.h"

#include <absl/container/inlined_vector.h>
#include <absl/strings/string_view.h>

#include <memory>

namespace lox {

using StatementsList = absl::InlinedVector<std::unique_ptr<Stmt>, 16>;

class Interpreter
    : public expr::Visitor
    , stmt::Visitor {
  ExprResult evaluate(std::shared_ptr<Expr> expr);
  void execute(Stmt &stmt) { stmt.accept(*this); }
  std::string to_string(ExprResult);

 public:
  ExprResult visitBoolLiteralExpr(BoolLiteral &l) override { return l.value_; }
  ExprResult visitNumLiteralExpr(NumLiteral &l) override { return l.value_; }
  ExprResult visitStrLiteralExpr(StrLiteral &l) override { return l.value_; }
  ExprResult visitGroupExpr(Group &g) override { return evaluate(g.expr_); }
  ExprResult visitBinaryExpr(Binary &b) override;
  ExprResult visitUnaryExpr(Unary &u) override;

  void visitExpressionStmt(Expression &e) override;

  void interpret(StatementsList &&l);
};

} // namespace lox

#endif // LOX_INTERPRETER_H_
