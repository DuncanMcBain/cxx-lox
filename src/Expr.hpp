#ifndef LOX_EXPR_HPP
#define LOX_EXPR_HPP

#include "Token.hpp"

#include <absl/container/inlined_vector.h>

#include <cstddef>
#include <memory>
#include <string>
#include <variant>

namespace lox {

class Callable;
using CallablePtr = std::shared_ptr<Callable>;

using ExprResult =
    std::variant<bool, double, std::string, std::nullptr_t, CallablePtr>;

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
struct Call;
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
  virtual T visitCallExpr(Call &)               = 0;
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

using ExprPtr         = std::shared_ptr<Expr>;
using ExpressionsList = absl::InlinedVector<std::shared_ptr<lox::Expr>, 8>;

struct Assign : Expr {
  Token name_;
  ExprPtr val_;
  Assign(Token name, ExprPtr val)
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
  ExprPtr left_;
  ExprPtr right_;
  Token op_;
  Binary(ExprPtr left, ExprPtr right, Token op)
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
  ExprPtr cond_;
  ExprPtr left_;
  ExprPtr right_;
  Ternary(ExprPtr cond, ExprPtr left, ExprPtr right)
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

struct Call : Expr {
  ExprPtr callee_;
  Token paren_;
  ExpressionsList args_;
  Call(ExprPtr callee, Token paren, ExpressionsList args)
      : callee_(callee)
      , paren_(paren)
      , args_(args) {}
  ExprResult accept(expr::Visitor<ExprResult> &v) override {
    return v.visitCallExpr(*this);
  }
  std::string accept(expr::Visitor<std::string> &v) override {
    return v.visitCallExpr(*this);
  }
};

struct Group : Expr {
  ExprPtr expr_;
  Group(ExprPtr expr)
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
  ExprPtr left_;
  ExprPtr right_;
  Token op_;
  Logical(ExprPtr left, ExprPtr right, Token op)
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
  ExprPtr right_;
  Token op_;
  Unary(ExprPtr right, Token op)
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
#endif // LOX_EXPR_HPP
