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

struct Visitor;

} // namespace stmt

struct Stmt {
  virtual void accept(stmt::Visitor &) = 0;
  virtual ~Stmt()                      = default;
};

struct Block;
struct Expression;
struct Var;

namespace stmt {

struct Visitor {
  virtual void visitBlockStmt(Block &)           = 0;
  virtual void visitExpressionStmt(Expression &) = 0;
  virtual void visitVarStmt(Var &)               = 0;
  virtual ~Visitor()                             = default;
};

} // namespace stmt

using StatementsList = absl::InlinedVector<std::unique_ptr<lox::Stmt>, 8>;

struct Block : Stmt {
  StatementsList statements_;
  Block(StatementsList&& statements) : statements_{std::move(statements)} {}
  void accept(stmt::Visitor &v) override { return v.visitBlockStmt(*this); }
};

struct Expression : Stmt {
  std::shared_ptr<Expr> expression_;
  Expression(std::shared_ptr<Expr> expression)
      : expression_(expression) {}
  void accept(stmt::Visitor &v) override {
    return v.visitExpressionStmt(*this);
  }
};

struct Var : Stmt {
  Token name_;
  std::shared_ptr<Expr> initialiser_;
  Var(Token name, std::shared_ptr<Expr> initialiser)
      : name_(name)
      , initialiser_(initialiser) {}
  void accept(stmt::Visitor &v) override { return v.visitVarStmt(*this); }
};

} // namespace lox
#endif
