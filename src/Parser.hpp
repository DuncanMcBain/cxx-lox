#ifndef LOX_PARSER_HPP
#define LOX_PARSER_HPP

#include "Expr.hpp"
#include "Stmt.hpp"
#include "Token.hpp"
#include "Utils.hpp"

#include <absl/container/inlined_vector.h>

#include <memory>

namespace lox {

using TokenTypeList = absl::InlinedVector<lox::TokenType, 4>;

class Parser {
  enum class FunctionKind { FUNC, METHOD };

  std::vector<Token> tokens_;
  int current_;
  bool parsing_args_;

  ExprPtr and_expr();
  ExprPtr assignment();
  ExprPtr call();
  ExprPtr comma();
  ExprPtr comparison();
  ExprPtr equality();
  ExprPtr expression();
  ExprPtr factor();
  ExprPtr or_expr();
  ExprPtr primary();
  ExprPtr term();
  ExprPtr ternary();
  ExprPtr unary();

  StatementsList block();
  StmtPtr declaration();
  StmtPtr exprstmt();
  StmtPtr for_stmt();
  StmtPtr function(FunctionKind);
  StmtPtr statement();
  StmtPtr var_declaration();
  StmtPtr while_stmt();

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
      , current_(0)
      , parsing_args_{false} {}

  StatementsList parse() {
    StatementsList statements;
    while (!at_end()) {
      auto decl = declaration();
      if (decl) statements.push_back(std::move(decl));
    }
    return statements;
  }
};

} // namespace lox

#endif // LOX_PARSER_HPP
