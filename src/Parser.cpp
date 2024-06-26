#include "Parser.h"
#include "Utils.h"

namespace lox {

bool Parser::match(const TokenTypeList &types) {
  for (auto type : types) {
    if (check(type)) {
      advance();
      return true;
    }
  }
  return false;
}

const Token &Parser::consume(TokenType type, absl::string_view msg) {
  if (check(type)) { return advance(); }
  throw ParseError(msg, peek().location());
}

void Parser::sync() {
  using enum TokenType;
  advance();
  while (!at_end()) {
    if (prev().type() == SEMICOLON) { return; }
    switch (peek().type()) {
    case CLASS:
    case FOR:
    case FUN:
    case IF:
    case PRINT:
    case RETURN:
    case VAR:
    case WHILE: return;
    default:;
    }
    advance();
  }
}

StatementsList Parser::block() {
  StatementsList stmts;
  while (!check(TokenType::R_BRACE) && !at_end())
    stmts.push_back(std::move(declaration()));
  consume(TokenType::R_BRACE, "Expected '}' after block statement");
  return stmts;
}

std::shared_ptr<Stmt> Parser::declaration() {
  try {
    if (match({TokenType::VAR})) {
      return var_declaration();
    }
    return statement();
  } catch (const ParseError &pe) {
    sync();
    return nullptr;
  }
}

std::shared_ptr<Stmt> Parser::var_declaration() {
  auto name = consume(TokenType::IDENT, "Expected variable name");
  auto initialiser = match({TokenType::EQ}) ? expression() : nullptr;
  consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
  return std::make_shared<Var>(name, initialiser);
}

std::shared_ptr<Stmt> Parser::statement() {
  using enum TokenType;
  if (match({IF})) {
    consume(L_PAREN, "Expected '(' after 'if'.");
    auto cond = expression();
    consume(R_PAREN, "Expected ')' after if condition.");
    auto then = statement();
    auto else_br = match({ELSE}) ? statement() : nullptr;
    return std::make_shared<If>(cond, then, else_br);
  }
  if (match({L_BRACE}))
    return std::shared_ptr<Block>(new Block(std::move(block())));
  return exprstmt();
}

std::shared_ptr<Stmt> Parser::exprstmt() {
  auto expr = expression();
  consume(TokenType::SEMICOLON, "Expected `;` after expression");
  return std::make_shared<Expression>(expr);
}

std::shared_ptr<Expr> Parser::expression() { return assignment(); }

std::shared_ptr<Expr> Parser::assignment() {
  auto expr = comma();

  if (match({TokenType::EQ})) {
    auto eq = prev();
    auto val = assignment();

    if (auto var = dynamic_cast<Variable*>(expr.get())) {
      auto name = var->name_;
      return std::make_shared<Assign>(name, val);
    }
    report_error("Invalid assignment target.", Location{});
  }
  return expr;
}

std::shared_ptr<Expr> Parser::comma() {
  auto expr = or_expr();
  while (match({TokenType::COMMA})) {
    auto op    = prev();
    auto right = equality();
    expr       = std::make_shared<Binary>(expr, right, op);
  }
  return expr;
}

std::shared_ptr<Expr> Parser::or_expr() {
  auto expr = and_expr();
  while (match({TokenType::OR})) {
    auto op = prev();
    auto right = and_expr();
    expr = std::make_shared<Logical>(expr, right, op);
  }
  return expr;
}

std::shared_ptr<Expr> Parser::and_expr() {
  auto expr = ternary();
  while (match({TokenType::AND})) {
    auto op = prev();
    auto right = ternary();
    expr = std::make_shared<Logical>(expr, right, op);
  }
  return expr;
}

std::shared_ptr<Expr> Parser::ternary() {
  using enum TokenType;
  auto cond = equality();
  if (match({QUESTION})) {
    auto left = equality();
    consume(COLON, "Expected ':' to match '?' in ternary expr");
    auto right = equality();
    return std::make_shared<Ternary>(cond, left, right);
  }
  return cond;
}

// TODO: make these all one instance of a template somehow? Or preprocessor?
std::shared_ptr<Expr> Parser::equality() {
  auto expr = comparison();
  while (match({TokenType::BANG_EQ, TokenType::EQ_EQ})) {
    auto op    = prev();
    auto right = comparison();
    expr       = std::make_shared<Binary>(expr, right, op);
  }
  return expr;
}

std::shared_ptr<Expr> Parser::comparison() {
  using enum TokenType;
  auto expr = term();
  while (match({GTR, GTR_EQ, LESS, LESS_EQ})) {
    auto op    = prev();
    auto right = term();
    expr       = std::make_shared<Binary>(expr, right, op);
  }
  return expr;
}

std::shared_ptr<Expr> Parser::term() {
  auto expr = factor();
  while (match({TokenType::MINUS, TokenType::PLUS})) {
    auto op    = prev();
    auto right = factor();
    expr       = std::make_shared<Binary>(expr, right, op);
  }
  return expr;
}

std::shared_ptr<Expr> Parser::factor() {
  auto expr = unary();
  while (match({TokenType::SLASH, TokenType::STAR})) {
    auto op    = prev();
    auto right = unary();
    expr       = std::make_shared<Binary>(expr, right, op);
  }
  return expr;
}

std::shared_ptr<Expr> Parser::unary() {
  if (match({TokenType::BANG, TokenType::MINUS})) {
    auto op    = prev();
    auto right = unary();
    return std::make_shared<Unary>(right, op);
  }
  return primary();
}

std::shared_ptr<Expr> Parser::primary() {
  using enum TokenType;
  if (match({FALSE})) { return std::make_shared<BoolLiteral>(false); }
  if (match({TRUE})) { return std::make_shared<BoolLiteral>(true); }
  if (match({NIL})) { return std::make_shared<NullLiteral>(); }
  if (match({NUMBER})) { return std::make_shared<NumLiteral>(prev().number()); }
  // TODO: change this to clarify ownership, what will live longest? Probably
  // the expression node so string should live in it, views everywhere else
  if (match({STRING})) { return std::make_shared<StrLiteral>(std::string(prev().string())); }

  if (match({IDENT})) { return std::make_shared<Variable>(prev()); }

  if (match({L_PAREN})) {
    auto expr = expression();
    consume(R_PAREN, "Expected ')' after expression to match '('");
    return std::make_shared<Group>(expr);
  }

  throw ParseError("Expected expression", tokens_[current_].location());
}

} // namespace lox
