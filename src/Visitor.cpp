#include "Expr.h"
#include "Stmt.h"

#include <exception>

namespace lox {

ExprResult expr::Visitor::visitBinaryExpr(Binary &) { throw std::exception(); }
ExprResult expr::Visitor::visitTernaryExpr(Ternary &) {
  throw std::exception();
}
ExprResult expr::Visitor::visitGroupExpr(Group &) { throw std::exception(); }
ExprResult expr::Visitor::visitBoolLiteralExpr(BoolLiteral &) {
  throw std::exception();
}
ExprResult expr::Visitor::visitStrLiteralExpr(StrLiteral &) {
  throw std::exception();
}
ExprResult expr::Visitor::visitNullLiteralExpr(NullLiteral &) {
  throw std::exception();
}
ExprResult expr::Visitor::visitNumLiteralExpr(NumLiteral &) {
  throw std::exception();
}
ExprResult expr::Visitor::visitUnaryExpr(Unary &) { throw std::exception(); }
void stmt::Visitor::visitExpressionStmt(Expression &) {
  throw std::exception();
}

} // namespace lox
