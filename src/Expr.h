#ifndef LOX_EXPR_H_
#define LOX_EXPR_H_

#include "Token.h"

#include <string>

namespace lox {

struct Visitor;
struct Expr {
  virtual void accept(Visitor &);
};
struct Binary;
struct Grouping;
struct StrLiteral;
struct NumLiteral;
struct Unary;
struct Visitor {
  void visitBinaryExpr(Binary &);
  void visitGroupingExpr(Grouping &);
  void visitStrLiteralExpr(StrLiteral &);
  void visitNumLiteralExpr(NumLiteral &);
  void visitUnaryExpr(Unary &);
};

struct Binary : Expr {
  Expr &left_;
  Expr &right_;
  Token op_;
  Binary(Expr &left, Expr &right, Token op)
      : left_(left)
      , right_(right)
      , op_(op) {}
  void accept(Visitor &v) override { v.visitBinaryExpr(*this); }
};

struct Grouping : Expr {
  Expr &expr_;
  Grouping(Expr &expr)
      : expr_(expr) {}
  void accept(Visitor &v) override { v.visitGroupingExpr(*this); }
};

struct StrLiteral : Expr {
  std::string value_;
  StrLiteral(std::string value)
      : value_(value) {}
  void accept(Visitor &v) override { v.visitStrLiteralExpr(*this); }
};

struct NumLiteral : Expr {
  double literal_;
  NumLiteral(double literal)
      : literal_(literal) {}
  void accept(Visitor &v) override { v.visitNumLiteralExpr(*this); }
};

struct Unary : Expr {
  Expr &right_;
  Token op_;
  Unary(Expr &right, Token op)
      : right_(right)
      , op_(op) {}
  void accept(Visitor &v) override { v.visitUnaryExpr(*this); }
};

} // namespace lox
#endif
