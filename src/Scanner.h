#ifndef LOX_SCANNER_H_
#define LOX_SCANNER_H_

#include "Token.h"

#include <absl/strings/string_view.h>

#include <unordered_map>
#include <vector>

namespace lox {

class Scanner {
  absl::string_view source_;
  std::vector<Token> tokens_;
  std::unordered_map<std::string, TokenType> identifiers_;
  size_t start_   = 0;
  size_t current_ = 0;
  int line_       = 1;

  char advance();
  void add_token(TokenType);
  void consume_identifier();
  void consume_number();
  void consume_string();
  bool match(char);
  char peek();
  char peek(size_t count);
  void scan_token();
  absl::string_view slice_token();

  bool at_end() { return current_ >= source_.length(); }

 public:
  Scanner(absl::string_view src)
      : source_(src)
      , tokens_{} {}

  std::vector<Token> &tokenise();
};

} // namespace lox

#endif // LOX_SCANNER_H_
