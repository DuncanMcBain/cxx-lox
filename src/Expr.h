#ifndef LOX_EXPR_H_
#define LOX_EXPR_H_

#include "Token.h"

#include <cstddef>
#include <memory>
#include <string>
#include <variant>

using ExprResult = std::variant<bool, double, std::string, std::nullptr_t>;

namespace lox {

struct Visitor;

struct Expr {
  virtual ExprResult accept(Visitor &) = 0;
  virtual ~Expr()                      = default;
};

struct Binary;
struct Ternary;
struct Grouping;
struct BoolLiteral;
struct StrLiteral;
struct NullLiteral;
struct NumLiteral;
struct Unary;

struct Visitor {
  virtual ExprResult visitBinaryExpr(Binary &);
  virtual ExprResult visitTernaryExpr(Ternary &);
  virtual ExprResult visitGroupingExpr(Grouping &);
  virtual ExprResult visitBoolLiteralExpr(BoolLiteral &);
  virtual ExprResult visitStrLiteralExpr(StrLiteral &);
  virtual ExprResult visitNullLiteralExpr(NullLiteral &);
  virtual ExprResult visitNumLiteralExpr(NumLiteral &);
  virtual ExprResult visitUnaryExpr(Unary &);
};

struct Binary : Expr {
  std::shared_ptr<Expr> left_;
  std::shared_ptr<Expr> right_;
  Token op_;
  Binary(std::shared_ptr<Expr> left, std::shared_ptr<Expr> right, Token op)
      : left_(left)
      , right_(right)
      , op_(op) {}
  ExprResult accept(Visitor &v) override { return v.visitBinaryExpr(*this); }
};

struct Ternary : Expr {
  std::shared_ptr<Expr> cond_;
  std::shared_ptr<Expr> left_;
  std::shared_ptr<Expr> right_;
  Ternary(std::shared_ptr<Expr> cond, std::shared_ptr<Expr> left,
          std::shared_ptr<Expr> right)
      : cond_(cond)
      , left_(left)
      , right_(right) {}
  ExprResult accept(Visitor &v) override { return v.visitTernaryExpr(*this); }
};

struct Grouping : Expr {
  std::shared_ptr<Expr> expr_;
  Grouping(std::shared_ptr<Expr> expr)
      : expr_(expr) {}
  ExprResult accept(Visitor &v) override { return v.visitGroupingExpr(*this); }
};

struct BoolLiteral : Expr {
  bool value_;
  BoolLiteral(bool value)
      : value_(value) {}
  ExprResult accept(Visitor &v) override {
    return v.visitBoolLiteralExpr(*this);
  }
};

struct StrLiteral : Expr {
  std::string value_;
  StrLiteral(std::string value)
      : value_(value) {}
  ExprResult accept(Visitor &v) override {
    return v.visitStrLiteralExpr(*this);
  }
};

struct NullLiteral : Expr {
  NullLiteral() {}
  ExprResult accept(Visitor &v) override {
    return v.visitNullLiteralExpr(*this);
  }
};

struct NumLiteral : Expr {
  double value_;
  NumLiteral(double value)
      : value_(value) {}
  ExprResult accept(Visitor &v) override {
    return v.visitNumLiteralExpr(*this);
  }
};

struct Unary : Expr {
  std::shared_ptr<Expr> right_;
  Token op_;
  Unary(std::shared_ptr<Expr> right, Token op)
      : right_(right)
      , op_(op) {}
  ExprResult accept(Visitor &v) override { return v.visitUnaryExpr(*this); }
};

} // namespace lox
#endif
