#include <absl/base/macros.h>

namespace lox {
[[noreturn]] inline void unreachable() {
  ABSL_ASSERT(false && "Unreachable reached!");
  __builtin_unreachable();
}
}
