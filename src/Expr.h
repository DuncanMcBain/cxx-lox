#ifndef LOX_EXPR_H_
#define LOX_EXPR_H_

#include "Token.h"

#include <cstddef>
#include <memory>
#include <string>
#include <variant>

namespace lox {

using ExprResult = std::variant<bool, double, std::string, std::nullptr_t>;

namespace expr {

template <typename T>
struct Visitor;

} // namespace expr

struct Expr {
  virtual ExprResult accept(expr::Visitor<ExprResult> &)   = 0;
  virtual std::string accept(expr::Visitor<std::string> &) = 0;
  virtual ~Expr()                                          = default;
};

struct Assign;
struct Binary;
struct Ternary;
struct Group;
struct BoolLiteral;
struct StrLiteral;
struct NullLiteral;
struct NumLiteral;
struct Logical;
struct Variable;
struct Unary;

namespace expr {

template <typename T>
struct Visitor {
  virtual T visitAssignExpr(Assign &)           = 0;
  virtual T visitBinaryExpr(Binary &)           = 0;
  virtual T visitTernaryExpr(Ternary &)         = 0;
  virtual T visitGroupExpr(Group &)             = 0;
  virtual T visitBoolLiteralExpr(BoolLiteral &) = 0;
  virtual T visitStrLiteralExpr(StrLiteral &)   = 0;
  virtual T visitNullLiteralExpr(NullLiteral &) = 0;
  virtual T visitNumLiteralExpr(NumLiteral &)   = 0;
  virtual T visitLogicalExpr(Logical &)         = 0;
  virtual T visitVariableExpr(Variable &)       = 0;
  virtual T visitUnaryExpr(Unary &)             = 0;
  virtual ~Visitor()                            = default;
};

} // namespace expr

struct Assign : Expr {
  Token name_;
  std::shared_ptr<Expr> val_;
  Assign(Token name, std::shared_ptr<Expr> val)
      : name_(name)
      , val_(val) {}
  ExprResult accept(expr::Visitor<ExprResult> &v) override {
    return v.visitAssignExpr(*this);
  }
  std::string accept(expr::Visitor<std::string> &v) override {
    return v.visitAssignExpr(*this);
  }
};

struct Binary : Expr {
  std::shared_ptr<Expr> left_;
  std::shared_ptr<Expr> right_;
  Token op_;
  Binary(std::shared_ptr<Expr> left, std::shared_ptr<Expr> right, Token op)
      : left_(left)
      , right_(right)
      , op_(op) {}
  ExprResult accept(expr::Visitor<ExprResult> &v) override {
    return v.visitBinaryExpr(*this);
  }
  std::string accept(expr::Visitor<std::string> &v) override {
    return v.visitBinaryExpr(*this);
  }
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
  ExprResult accept(expr::Visitor<ExprResult> &v) override {
    return v.visitTernaryExpr(*this);
  }
  std::string accept(expr::Visitor<std::string> &v) override {
    return v.visitTernaryExpr(*this);
  }
};

struct Group : Expr {
  std::shared_ptr<Expr> expr_;
  Group(std::shared_ptr<Expr> expr)
      : expr_(expr) {}
  ExprResult accept(expr::Visitor<ExprResult> &v) override {
    return v.visitGroupExpr(*this);
  }
  std::string accept(expr::Visitor<std::string> &v) override {
    return v.visitGroupExpr(*this);
  }
};

struct BoolLiteral : Expr {
  bool value_;
  BoolLiteral(bool value)
      : value_(value) {}
  ExprResult accept(expr::Visitor<ExprResult> &v) override {
    return v.visitBoolLiteralExpr(*this);
  }
  std::string accept(expr::Visitor<std::string> &v) override {
    return v.visitBoolLiteralExpr(*this);
  }
};

struct StrLiteral : Expr {
  std::string value_;
  StrLiteral(std::string value)
      : value_(value) {}
  ExprResult accept(expr::Visitor<ExprResult> &v) override {
    return v.visitStrLiteralExpr(*this);
  }
  std::string accept(expr::Visitor<std::string> &v) override {
    return v.visitStrLiteralExpr(*this);
  }
};

struct NullLiteral : Expr {
  NullLiteral() {}
  ExprResult accept(expr::Visitor<ExprResult> &v) override {
    return v.visitNullLiteralExpr(*this);
  }
  std::string accept(expr::Visitor<std::string> &v) override {
    return v.visitNullLiteralExpr(*this);
  }
};

struct NumLiteral : Expr {
  double value_;
  NumLiteral(double value)
      : value_(value) {}
  ExprResult accept(expr::Visitor<ExprResult> &v) override {
    return v.visitNumLiteralExpr(*this);
  }
  std::string accept(expr::Visitor<std::string> &v) override {
    return v.visitNumLiteralExpr(*this);
  }
};

struct Logical : Expr {
  std::shared_ptr<Expr> left_;
  std::shared_ptr<Expr> right_;
  Token op_;
  Logical(std::shared_ptr<Expr> left, std::shared_ptr<Expr> right, Token op)
      : left_(left)
      , right_(right)
      , op_(op) {}
  ExprResult accept(expr::Visitor<ExprResult> &v) override {
    return v.visitLogicalExpr(*this);
  }
  std::string accept(expr::Visitor<std::string> &v) override {
    return v.visitLogicalExpr(*this);
  }
};

struct Variable : Expr {
  Token name_;
  Variable(Token name)
      : name_(name) {}
  ExprResult accept(expr::Visitor<ExprResult> &v) override {
    return v.visitVariableExpr(*this);
  }
  std::string accept(expr::Visitor<std::string> &v) override {
    return v.visitVariableExpr(*this);
  }
};

struct Unary : Expr {
  std::shared_ptr<Expr> right_;
  Token op_;
  Unary(std::shared_ptr<Expr> right, Token op)
      : right_(right)
      , op_(op) {}
  ExprResult accept(expr::Visitor<ExprResult> &v) override {
    return v.visitUnaryExpr(*this);
  }
  std::string accept(expr::Visitor<std::string> &v) override {
    return v.visitUnaryExpr(*this);
  }
};

} // namespace lox
#endif
