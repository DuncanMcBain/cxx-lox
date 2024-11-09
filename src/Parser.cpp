#include "Parser.h"
#include "Error.h"
#include "Utils.h"

#include <fmt/format.h>

#include <initializer_list>

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

StmtPtr Parser::declaration() {
  try {
    if (match({TokenType::FUN})) { return function(FunctionKind::FUNC); }
    if (match({TokenType::VAR})) { return var_declaration(); }
    return statement();
  } catch (const ParseError &pe) {
    sync();
    report_error(pe.what(), Location{}.chr(current_));
    return nullptr;
  }
}

StmtPtr Parser::var_declaration() {
  auto name        = consume(TokenType::IDENT, "Expected variable name");
  auto initialiser = match({TokenType::EQ}) ? expression() : nullptr;
  consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
  return std::make_shared<Var>(name, initialiser);
}

StmtPtr Parser::statement() {
  using enum TokenType;
  if (match({FOR})) return for_stmt();
  if (match({IF})) {
    consume(L_PAREN, "Expected '(' after 'if'.");
    auto cond = expression();
    consume(R_PAREN, "Expected ')' after if condition.");
    auto then    = statement();
    auto else_br = match({ELSE}) ? statement() : nullptr;
    return std::make_shared<If>(cond, then, else_br);
  }
  if (match({WHILE})) return while_stmt();
  if (match({L_BRACE}))
    return std::shared_ptr<Block>(new Block(std::move(block())));
  return exprstmt();
}

StmtPtr Parser::for_stmt() {
  using enum TokenType;
  consume(L_PAREN, "Expected '(' after 'for'");

  StmtPtr init = nullptr;
  if (match({SEMICOLON})) {
    // nothing happens, there's no intialiser
  } else if (match({VAR})) {
    init = var_declaration();
  } else {
    init = exprstmt();
  }

  ExprPtr condition = nullptr;
  if (!check(SEMICOLON)) condition = expression();
  consume(SEMICOLON, "Expected ';' after loop condition.");

  ExprPtr increment = nullptr;
  if (!check(R_PAREN)) increment = expression();
  consume(R_PAREN, "Expected ')' after for loop clauses.");
  StmtPtr body = statement();

  if (increment) {
    body = std::make_shared<Block>(
        StatementsList({body, std::make_shared<Expression>(increment)}));
  }
  if (!condition) condition = std::make_shared<BoolLiteral>(true);
  body = std::make_shared<While>(condition, body);
  if (init) body = std::make_shared<Block>(StatementsList({init, body}));

  return body;
}

StmtPtr Parser::function(FunctionKind kind) {
  using enum TokenType;
  auto str = [](FunctionKind k) {
    using enum FunctionKind;
    switch (k) {
    case FUNC: return "function";
    case METHOD: return "class method";
    default: return "unknown function kind";
    }
  };
  auto kind_str = str(kind);
  auto name     = consume(IDENT, fmt::format("Expected {} name.", kind_str));
  auto name_str = name.identifier();
  consume(L_PAREN, fmt::format("Expected '(' after {} identifier (name {}).",
                               kind_str, name_str));
  TokensList params;
  if (!check(R_PAREN)) {
    do {
      if (params.size() >= 255)
        report_error("Exceeded maximum parameter count of 254", Location{});
      params.push_back(
          consume(IDENT, fmt::format("Expected parameter name in {} {}.",
                                     kind_str, name_str)));
    } while (match({COMMA}));
  }
  consume(R_PAREN,
          fmt::format("Expected ')' after {} parameter list.", name_str));
  consume(L_BRACE,
          fmt::format("Expected '{{' before {} {} body.", kind_str, name_str));
  auto body = block();
  return std::make_shared<Fn>(name, params, body);
}

StmtPtr Parser::while_stmt() {
  consume(TokenType::L_PAREN, "Expected '(' after 'while'.");
  auto cond = expression();
  consume(TokenType::R_PAREN, "Expected ')' after while condition.");
  auto body = statement();
  return std::make_shared<While>(cond, body);
}

StmtPtr Parser::exprstmt() {
  auto expr = expression();
  consume(TokenType::SEMICOLON, "Expected `;` after expression");
  return std::make_shared<Expression>(expr);
}

ExprPtr Parser::expression() { return assignment(); }

ExprPtr Parser::assignment() {
  auto expr = comma();

  if (match({TokenType::EQ})) {
    auto eq  = prev();
    auto val = assignment();

    if (auto var = dynamic_cast<Variable *>(expr.get())) {
      auto name = var->name_;
      return std::make_shared<Assign>(name, val);
    }
    report_error("Invalid assignment target.", Location{});
  }
  return expr;
}

ExprPtr Parser::comma() {
  auto expr = or_expr();
  // Use short-circuit evaluation so we don't eat the comma
  while (!parsing_args_ && match({TokenType::COMMA})) {
    auto op    = prev();
    auto right = equality();
    expr       = std::make_shared<Binary>(expr, right, op);
  }
  return expr;
}

ExprPtr Parser::or_expr() {
  auto expr = and_expr();
  while (match({TokenType::OR})) {
    auto op    = prev();
    auto right = and_expr();
    expr       = std::make_shared<Logical>(expr, right, op);
  }
  return expr;
}

ExprPtr Parser::and_expr() {
  auto expr = ternary();
  while (match({TokenType::AND})) {
    auto op    = prev();
    auto right = ternary();
    expr       = std::make_shared<Logical>(expr, right, op);
  }
  return expr;
}

ExprPtr Parser::ternary() {
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
ExprPtr Parser::equality() {
  auto expr = comparison();
  while (match({TokenType::BANG_EQ, TokenType::EQ_EQ})) {
    auto op    = prev();
    auto right = comparison();
    expr       = std::make_shared<Binary>(expr, right, op);
  }
  return expr;
}

ExprPtr Parser::comparison() {
  using enum TokenType;
  auto expr = term();
  while (match({GTR, GTR_EQ, LESS, LESS_EQ})) {
    auto op    = prev();
    auto right = term();
    expr       = std::make_shared<Binary>(expr, right, op);
  }
  return expr;
}

ExprPtr Parser::term() {
  auto expr = factor();
  while (match({TokenType::MINUS, TokenType::PLUS})) {
    auto op    = prev();
    auto right = factor();
    expr       = std::make_shared<Binary>(expr, right, op);
  }
  return expr;
}

ExprPtr Parser::factor() {
  auto expr = unary();
  while (match({TokenType::SLASH, TokenType::STAR})) {
    auto op    = prev();
    auto right = unary();
    expr       = std::make_shared<Binary>(expr, right, op);
  }
  return expr;
}

ExprPtr Parser::unary() {
  if (match({TokenType::BANG, TokenType::MINUS})) {
    auto op    = prev();
    auto right = unary();
    return std::make_shared<Unary>(right, op);
  }
  return call();
}

ExprPtr Parser::call() {
  ExprPtr expr = primary();

  auto get_args = [this](ExprPtr callee) {
    using enum TokenType;
    ExpressionsList args;
    bool already_reported = false;
    parsing_args_         = true;
    if (!check(R_PAREN)) {
      do {
        if (args.size() >= 255 && !already_reported) {
          report_error("Functions must not exceed 255 arguments.", Location{});
          already_reported = true;
        }
        args.push_back(expression());
      } while (match({COMMA}));
    }
    parsing_args_ = false;
    auto paren    = consume(R_PAREN, "Expected ')' after argument list.");
    return std::make_shared<Call>(callee, paren, args);
  };

  while (true) {
    if (match({TokenType::L_PAREN})) {
      expr = get_args(expr);
    } else {
      break;
    }
  }
  return expr;
}

ExprPtr Parser::primary() {
  using enum TokenType;
  if (match({FALSE})) { return std::make_shared<BoolLiteral>(false); }
  if (match({TRUE})) { return std::make_shared<BoolLiteral>(true); }
  if (match({NIL})) { return std::make_shared<NullLiteral>(); }
  if (match({NUMBER})) { return std::make_shared<NumLiteral>(prev().number()); }
  // TODO: change this to clarify ownership, what will live longest? Probably
  // the expression node so string should live in it, views everywhere else
  if (match({STRING})) {
    return std::make_shared<StrLiteral>(std::string(prev().string()));
  }

  if (match({IDENT})) { return std::make_shared<Variable>(prev()); }

  if (match({L_PAREN})) {
    auto expr = expression();
    consume(R_PAREN, "Expected ')' after expression to match '('");
    return std::make_shared<Group>(expr);
  }

  throw ParseError("Expected expression", tokens_[current_].location());
}

} // namespace lox
