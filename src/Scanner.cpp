#include "Scanner.h"
#include "Error.h"

#include <absl/base/macros.h>
#include <absl/strings/ascii.h>

namespace lox {

void Scanner::add_token(TokenType type) {
  tokens_.emplace_back(type, slice_token(), line_, current_);
}

char Scanner::advance() { return source_.at(current_++); }

void Scanner::consume_identifier() {
  while (absl::ascii_isalnum(peek()) || peek() == '_') { advance(); }

  auto ident = slice_token();
  auto type  = TokenType::IDENT;
  if (keywords_.find(ident) != keywords_.end()) { type = keywords_.at(ident); }
  add_token(type);
}

void Scanner::consume_number() {
  while (absl::ascii_isdigit(peek())) { advance(); }

  // TODO: consider supporting 2.f style numbers
  if (peek() == '.' && absl::ascii_isdigit(peek(1))) {
    do { advance(); } while (absl::ascii_isdigit(peek()));
  }
  add_token(TokenType::NUMBER);
}

void Scanner::consume_string() {
  while (peek() != '"' && !at_end()) {
    if (peek() == '\n') { line_++; }
    advance();
  }

  if (at_end()) {
    report_error("Unterminated string", Location{}.line(line_));
    return;
  }

  // TODO: add escape sequences here
  advance();
  add_token(TokenType::STRING);
}

bool Scanner::match(char expected) {
  if (at_end()) { return false; }
  if (source_.at(current_) != expected) { return false; }
  current_++;
  return true;
}

char Scanner::peek() const { return at_end() ? '\0' : source_.at(current_); }

char Scanner::peek(size_t c) const {
  return current_ + c < source_.length() ? source_.at(current_ + c) : '\0';
}

void Scanner::scan_token() {
  using enum TokenType;
  auto chr = advance();
  switch (chr) {
  case '(': add_token(L_PAREN); break;
  case ')': add_token(R_PAREN); break;
  case '{': add_token(L_BRACE); break;
  case '}': add_token(R_BRACE); break;
  case ',': add_token(COMMA); break;
  case '.': add_token(DOT); break;
  case '-': add_token(MINUS); break;
  case '+': add_token(PLUS); break;
  case ';': add_token(SEMICOLON); break;
  case '*': add_token(STAR); break;
  case '?': add_token(QUESTION); break;
  case ':': add_token(COLON); break;
  case '!': add_token(match('=') ? BANG_EQ : BANG); break;
  case '=': add_token(match('=') ? EQ_EQ : EQ); break;
  case '<': add_token(match('=') ? LESS_EQ : LESS); break;
  case '>': add_token(match('=') ? GTR_EQ : GTR); break;
  case '/':
    // case for comments running to eol
    if (match('/')) {
      while (peek() != '\n' && !at_end()) { advance(); }
    } else {
      add_token(SLASH);
    }
    break;
  case ' ':
  case '\t':
  case '\r': break;
  case '\n': line_++;
  case '"': consume_string(); break;
  default:
    if (absl::ascii_isdigit(chr)) {
      consume_number();
    } else if (absl::ascii_isalpha(chr)) {
      consume_identifier();
    } else {
      report_error("Unexpected character",
                   Location{}.line(line_).chr(current_));
    }
    break;
  }
}

absl::string_view Scanner::slice_token() const {
  auto b = source_.begin();
  return {b + start_, b + current_};
}

std::vector<Token> &Scanner::tokenise() {
  while (!at_end()) {
    start_ = current_;
    scan_token();
  }
  tokens_.emplace_back(TokenType::EOF, "", line_, current_ + 1);
  return tokens_;
}

} // namespace lox
