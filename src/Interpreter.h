#ifndef LOX_INTERPRETER_H_
#define LOX_INTERPRETER_H_

#include "Expr.h"
#include "Utils.h"

#include <absl/strings/string_view.h>

#include <memory>

namespace lox {

class Interpreter : public Visitor {
  ExprResult evaluate(std::shared_ptr<Expr> expr);
  std::string to_string(ExprResult);
 public:
  ExprResult visitBoolLiteralExpr(BoolLiteral &l) { return l.value_; }
  ExprResult visitNumLiteralExpr(NumLiteral &l) { return l.value_; }
  ExprResult visitStrLiteralExpr(StrLiteral &l) { return l.value_; }
  ExprResult visitGroupExpr(Grouping &group) { return evaluate(group.expr_); }
  ExprResult visitBinaryExpr(Binary &b);
  ExprResult visitUnaryExpr(Unary &u);

  void interpret(std::shared_ptr<Expr>);
};

} // namespace lox

#endif // LOX_INTERPRETER_H_
