#ifndef LOX_KEYWORDNAMES_H_
#define LOX_KEYWORDNAMES_H_

#include "TokenTypes.h"

#include <absl/strings/string_view.h>

#include <unordered_map>

namespace lox {

const std::unordered_map<absl::string_view, TokenType> &get_keywords();

} // namespace lox

#endif // LOX_KEYWORDNAMES_H_
