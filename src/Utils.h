#ifndef LOX_UTILS_H_
#define LOX_UTILS_H_
#include <absl/base/macros.h>

namespace lox {
[[noreturn]] inline void unreachable() {
  ABSL_ASSERT(false && "Unreachable reached!");
  __builtin_unreachable();
}

template <typename T>
class Reverse {
  T &iter_;

public:
  explicit Reverse(T &iter)
      : iter_{iter} {}

  auto begin() const { return std::rbegin(iter_); }
  auto end() const { return std::rend(iter_); }
};

template <typename T>
auto make_reverse(T &iter) {
  return Reverse<T>(iter);
}

} // namespace lox
#endif // LOX_UTILS_H_
