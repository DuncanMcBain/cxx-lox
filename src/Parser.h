#ifndef LOX_PARSER_H_
#define LOX_PARSER_H_

#include "Expr.h"
#include "Stmt.h"
#include "Token.h"
#include "Utils.h"

#include <absl/container/inlined_vector.h>

#include <memory>

namespace lox {

using TokenTypeList  = absl::InlinedVector<lox::TokenType, 4>;
using StatementsList = absl::InlinedVector<std::unique_ptr<lox::Stmt>, 16>;

class Parser {
  std::vector<Token> tokens_;
  int current_;

  std::shared_ptr<Expr> assignment();
  std::shared_ptr<Expr> comma();
  std::shared_ptr<Expr> comparison();
  std::shared_ptr<Expr> equality();
  std::shared_ptr<Expr> expression();
  std::shared_ptr<Expr> factor();
  std::shared_ptr<Expr> primary();
  std::shared_ptr<Expr> term();
  std::shared_ptr<Expr> ternary();
  std::shared_ptr<Expr> unary();

  std::unique_ptr<Stmt> declaration();
  std::unique_ptr<Stmt> exprstmt();
  std::unique_ptr<Stmt> statement();
  std::unique_ptr<Stmt> var_declaration();

  const Token &consume(TokenType, absl::string_view);
  bool match(const TokenTypeList &);
  void sync();

  const Token &advance() { return tokens_[at_end() ? current_ : current_++]; }

  bool at_end() const { return peek().type() == TokenType::EOF; }

  bool check(TokenType type) const {
    if (at_end()) { return false; }
    return peek().type() == type;
  }

  const Token &peek() const { return tokens_[current_]; }

  const Token &prev() const {
    ABSL_ASSERT(current_ != 0);
    return tokens_[current_ - 1];
  }

 public:
  Parser(std::vector<Token> &&tokens)
      : tokens_(tokens)
      , current_(0) {}

  StatementsList parse() {
    StatementsList statements;
    while (!at_end()) { statements.push_back(declaration()); }
    return statements;
  }
};

} // namespace lox

#endif // LOX_PARSER_H_
