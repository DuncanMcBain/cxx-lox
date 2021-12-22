#ifndef LOX_SCANNER_H_
#define LOX_SCANNER_H_

#include "KeywordNames.h"
#include "Token.h"

#include <absl/strings/string_view.h>

#include <unordered_map>
#include <vector>

namespace lox {

class Scanner {
  const absl::string_view source_;
  std::vector<Token> tokens_;
  const std::unordered_map<absl::string_view, TokenType> &keywords_;
  size_t start_   = 0;
  size_t current_ = 0;
  int line_       = 1;

  char advance();
  void add_token(TokenType);
  void consume_identifier();
  void consume_number();
  void consume_string();
  bool match(char);
  char peek() const;
  char peek(size_t count) const;
  void scan_token();
  absl::string_view slice_token() const;

  bool at_end() const { return current_ >= source_.length(); }

 public:
  Scanner(absl::string_view src)
      : source_(src)
      , tokens_{}
      , keywords_(get_keywords()) {}

  std::vector<Token> &tokenise();
};

} // namespace lox

#endif // LOX_SCANNER_H_
