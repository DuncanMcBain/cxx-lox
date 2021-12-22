#include "KeywordNames.h"

#define X(TOKEN, token) \
  if (TOKEN == TokenType::AND) { modifying = true;  } \
  if (TOKEN == TokenType::EOF) { modifying = false; } \
  if (modifying) { keywords[ #token ] = TOKEN; }

namespace lox {

static std::unordered_map<absl::string_view, TokenType> keywords;

const std::unordered_map<absl::string_view, lox::TokenType>& get_keywords() {
  using enum TokenType;
  if (!keywords.empty()) {
    return keywords;
  }

  bool modifying = false;
#include "TokenTypes.inc"

  return keywords;
}

} // namespace lox
