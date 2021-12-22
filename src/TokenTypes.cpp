#include <vector>

#include "TokenTypes.h"

namespace {
#define X(NAME, _) #NAME,

static std::vector<std::string> names = {
#include "TokenTypes.inc"
};
} // namespace

namespace lox {

const std::string &to_string(TokenType type) {
  return names[static_cast<int>(type)];
}

} // namespace lox
