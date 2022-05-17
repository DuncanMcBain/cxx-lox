#ifndef LOX_STMT_H_
#define LOX_STMT_H_

#include "Expr.h"
#include "Token.h"

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

struct Expression;

namespace stmt {

struct Visitor {
  virtual void visitExpressionStmt(Expression &);
};

} // namespace stmt

struct Expression : Stmt {
  std::shared_ptr<Expr> expression_;
  Expression(std::shared_ptr<Expr> expression)
      : expression_(expression) {}
  void accept(stmt::Visitor &v) override {
    return v.visitExpressionStmt(*this);
  }
};

} // namespace lox
#endif
