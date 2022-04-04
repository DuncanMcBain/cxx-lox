#ifndef LOX_TOKEN_H_
#define LOX_TOKEN_H_

#include "Error.h"
#include "TokenTypes.h"

#include <absl/strings/numbers.h>
#include <absl/strings/string_view.h>

namespace lox {

class Token {
  TokenType type_;
  absl::string_view lexeme_;
  std::string identifier_;
  std::string string_;
  double number_ = 0.;
  int line_;
  size_t offset_;

 public:
  Token(TokenType type, absl::string_view lexeme, int line, size_t offset)
      : type_(type)
      , lexeme_(lexeme)
      , line_(line)
      , offset_(offset) {
    switch (type_) {
    case TokenType::IDENT: identifier_ = lexeme_; break;
    case TokenType::STRING:
      string_ =
          lexeme_ == "\"\"" ? "" : lexeme_.substr(1, lexeme_.length() - 2);
      break;
    case TokenType::NUMBER:
      if (!absl::SimpleAtod(lexeme_, &number_)) {
        report_error("Number out of range?", Location{}.line(line_));
      };
      break;
    default: break; // Do nothing
    }
  }

  auto type() const { return type_; }

  auto number() const { return number_; }

  auto string() const { return string_; }

  auto location() const {
    return Location{}.line(line_).chr(offset_).end_chr(offset_ +
                                                       lexeme_.length());
  }

  // TODO: use abseil string builders
  operator std::string() const {
    std::string val;
    val.append(to_string(type_));
    val.append(" ");
    switch (type_) {
    case TokenType::IDENT: val.append(identifier_); break;
    case TokenType::STRING: val.append(string_); break;
    case TokenType::NUMBER: val.append(std::to_string(number_)); break;
    default: break;
    }
    val.append(" lexeme ");
    val.append(lexeme_);
    return val;
  }
};

} //  namespace lox

#endif // LOX_TOKEN_H_
