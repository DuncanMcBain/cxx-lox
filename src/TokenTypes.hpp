#ifndef LOX_TOKENTYPES_HPP
#define LOX_TOKENTYPES_HPP

#include <string>

// This fixes a problem with fmtlib and C IO
#undef EOF

namespace lox {

#define X(name, _) name,

enum class TokenType : int {
#include "TokenTypes.inc"
};

const std::string &to_string(TokenType);

} // namespace lox

#endif // LOX_TOKENTYPES_HPP
