#ifndef LOX_PARSER_H_
#define LOX_PARSER_H_

#include "Expr.h"
#include "Token.h"

#include <absl/container/inlined_vector.h>

#include <memory>

using TokenTypeList = absl::InlinedVector<lox::TokenType, 4>;

namespace lox {

class Parser {
  std::vector<Token> tokens_;
  int current_;

  std::shared_ptr<Expr> comma();
  std::shared_ptr<Expr> comparison();
  std::shared_ptr<Expr> equality();
  std::shared_ptr<Expr> expression();
  std::shared_ptr<Expr> factor();
  std::shared_ptr<Expr> primary();
  std::shared_ptr<Expr> term();
  std::shared_ptr<Expr> ternary();
  std::shared_ptr<Expr> unary();

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

  // TODO: using statement Expression = shared_ptr<Expr>
  std::shared_ptr<Expr> parse() {
    try {
      return expression();
    } catch (const ParseError &error) {
      report_error(error.what(), error.location());
      return nullptr;
    }
  }
};

} // namespace lox

#endif // LOX_PARSER_H_
