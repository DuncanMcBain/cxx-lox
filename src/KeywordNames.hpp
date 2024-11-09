#ifndef LOX_KEYWORDNAMES_HPP
#define LOX_KEYWORDNAMES_HPP

#include "TokenTypes.hpp"

#include <absl/strings/string_view.h>

#include <unordered_map>

namespace lox {

const std::unordered_map<absl::string_view, TokenType> &get_keywords();

} // namespace lox

#endif // LOX_KEYWORDNAMES_HPP
