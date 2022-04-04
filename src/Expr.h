#ifndef LOX_EXPR_H_
#define LOX_EXPR_H_

#include "Token.h"

#include <memory>
#include <string>

namespace lox {

struct Visitor;

struct Expr {
  virtual void accept(Visitor &) = 0;
  virtual ~Expr()                = default;
};

struct Binary;
struct Grouping;
struct BoolLiteral;
struct StrLiteral;
struct NullLiteral;
struct NumLiteral;
struct Unary;

struct Visitor {
  virtual void visitBinaryExpr(Binary &);
  virtual void visitGroupingExpr(Grouping &);
  virtual void visitBoolLiteralExpr(BoolLiteral &);
  virtual void visitStrLiteralExpr(StrLiteral &);
  virtual void visitNullLiteralExpr(NullLiteral &);
  virtual void visitNumLiteralExpr(NumLiteral &);
  virtual void visitUnaryExpr(Unary &);
};

struct Binary : Expr {
  std::shared_ptr<Expr> left_;
  std::shared_ptr<Expr> right_;
  Token op_;
  Binary(std::shared_ptr<Expr> left, std::shared_ptr<Expr> right, Token op)
      : left_(left)
      , right_(right)
      , op_(op) {}
  void accept(Visitor &v) override { v.visitBinaryExpr(*this); }
};

struct Grouping : Expr {
  std::shared_ptr<Expr> expr_;
  Grouping(std::shared_ptr<Expr> expr)
      : expr_(expr) {}
  void accept(Visitor &v) override { v.visitGroupingExpr(*this); }
};

struct BoolLiteral : Expr {
  bool value_;
  BoolLiteral(bool value)
      : value_(value) {}
  void accept(Visitor &v) override { v.visitBoolLiteralExpr(*this); }
};

struct StrLiteral : Expr {
  std::string value_;
  StrLiteral(std::string value)
      : value_(value) {}
  void accept(Visitor &v) override { v.visitStrLiteralExpr(*this); }
};

struct NullLiteral : Expr {
  NullLiteral() {}
  void accept(Visitor &v) override { v.visitNullLiteralExpr(*this); }
};

struct NumLiteral : Expr {
  double literal_;
  NumLiteral(double literal)
      : literal_(literal) {}
  void accept(Visitor &v) override { v.visitNumLiteralExpr(*this); }
};

struct Unary : Expr {
  std::shared_ptr<Expr> right_;
  Token op_;
  Unary(std::shared_ptr<Expr> right, Token op)
      : right_(right)
      , op_(op) {}
  void accept(Visitor &v) override { v.visitUnaryExpr(*this); }
};

} // namespace lox
#endif
