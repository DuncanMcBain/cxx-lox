#ifndef LOX_TOKEN_H_
#define LOX_TOKEN_H_

#include "Error.h"
#include "TokenTypes.h"

#include <absl/strings/numbers.h>
#include <absl/strings/str_cat.h>
#include <absl/strings/string_view.h>

namespace lox {

class Token {
  TokenType type_;
  absl::string_view lexeme_;
  absl::string_view identifier_;
  absl::string_view string_;
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
  auto lexeme() const { return lexeme_; }
  auto identifier() const { return identifier_; }
  auto string() const { return string_; }
  auto number() const { return number_; }

  auto location() const {
    return Location{}.line(line_).chr(offset_).end_chr(offset_ +
                                                       lexeme_.length());
  }

  operator std::string() const {
    auto value = [&]() -> std::string {
      switch (type_) {
        case TokenType::IDENT: return std::string(identifier_);
        case TokenType::STRING: return std::string(string_);
      case TokenType::NUMBER: return std::to_string(number_);
      default: return "";
      }
    }();
    return absl::StrCat(to_string(type_), " ", value, " lexeme ", lexeme_);
  }
};

} //  namespace lox

#endif // LOX_TOKEN_H_
