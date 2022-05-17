#ifndef LOX_UTILS_H_
#define LOX_UTILS_H_
#include <absl/base/macros.h>

namespace lox {
[[noreturn]] inline void unreachable() {
  ABSL_ASSERT(false && "Unreachable reached!");
  __builtin_unreachable();
}
} // namespace lox
#endif // LOX_UTILS_H_
