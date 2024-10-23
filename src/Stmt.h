#ifndef LOX_STMT_H_
#define LOX_STMT_H_

#include "Expr.h"
#include "Token.h"

#include <absl/container/inlined_vector.h>

#include <cstddef>
#include <memory>
#include <string>
#include <variant>

namespace lox {

namespace stmt {

template <typename T>
struct Visitor;

} // namespace stmt

struct Stmt {
  virtual void accept(stmt::Visitor<void> &)               = 0;
  virtual std::string accept(stmt::Visitor<std::string> &) = 0;
  virtual ~Stmt()                                          = default;
};

struct Block;
struct Expression;
struct If;
struct While;
struct Var;

namespace stmt {

template <typename T>
struct Visitor {
  virtual T visitBlockStmt(Block &)           = 0;
  virtual T visitExpressionStmt(Expression &) = 0;
  virtual T visitIfStmt(If &)                 = 0;
  virtual T visitWhileStmt(While &)           = 0;
  virtual T visitVarStmt(Var &)               = 0;
  virtual ~Visitor()                          = default;
};

} // namespace stmt

using StatementsList = absl::InlinedVector<std::shared_ptr<lox::Stmt>, 8>;

struct Block : Stmt {
  StatementsList statements_;
  Block(StatementsList &&statements)
      : statements_(std::move(statements)) {}
  void accept(stmt::Visitor<void> &v) override {
    return v.visitBlockStmt(*this);
  }
  std::string accept(stmt::Visitor<std::string> &v) override {
    return v.visitBlockStmt(*this);
  }
};

struct Expression : Stmt {
  std::shared_ptr<Expr> expression_;
  Expression(std::shared_ptr<Expr> expression)
      : expression_(expression) {}
  void accept(stmt::Visitor<void> &v) override {
    return v.visitExpressionStmt(*this);
  }
  std::string accept(stmt::Visitor<std::string> &v) override {
    return v.visitExpressionStmt(*this);
  }
};

struct If : Stmt {
  std::shared_ptr<Expr> condition_;
  std::shared_ptr<Stmt> then_;
  std::shared_ptr<Stmt> else_br_;
  If(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> then,
     std::shared_ptr<Stmt> else_br)
      : condition_(condition)
      , then_(then)
      , else_br_(else_br) {}
  void accept(stmt::Visitor<void> &v) override { return v.visitIfStmt(*this); }
  std::string accept(stmt::Visitor<std::string> &v) override {
    return v.visitIfStmt(*this);
  }
};

struct While : Stmt {
  std::shared_ptr<Expr> condition_;
  std::shared_ptr<Stmt> body_;
  While(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body)
      : condition_(condition)
      , body_(body) {}
  void accept(stmt::Visitor<void> &v) override {
    return v.visitWhileStmt(*this);
  }
  std::string accept(stmt::Visitor<std::string> &v) override {
    return v.visitWhileStmt(*this);
  }
};

struct Var : Stmt {
  Token name_;
  std::shared_ptr<Expr> initialiser_;
  Var(Token name, std::shared_ptr<Expr> initialiser)
      : name_(name)
      , initialiser_(initialiser) {}
  void accept(stmt::Visitor<void> &v) override { return v.visitVarStmt(*this); }
  std::string accept(stmt::Visitor<std::string> &v) override {
    return v.visitVarStmt(*this);
  }
};

} // namespace lox
#endif
