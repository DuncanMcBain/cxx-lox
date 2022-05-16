#include "Expr.h"

namespace lox {

ExprResult Visitor::visitBinaryExpr(Binary&) {}
ExprResult Visitor::visitTernaryExpr(Ternary&) {}
ExprResult Visitor::visitGroupingExpr(Grouping&) {}
ExprResult Visitor::visitBoolLiteralExpr(BoolLiteral&) {}
ExprResult Visitor::visitStrLiteralExpr(StrLiteral&) {}
ExprResult Visitor::visitNullLiteralExpr(NullLiteral&) {}
ExprResult Visitor::visitNumLiteralExpr(NumLiteral&) {}
ExprResult Visitor::visitUnaryExpr(Unary&) {}
}
