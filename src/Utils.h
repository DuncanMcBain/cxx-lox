#ifndef LOX_UTILS_H_
#define LOX_UTILS_H_
#include <absl/base/macros.h>

namespace lox {
[[noreturn]] inline void unreachable() {
  ABSL_ASSERT(false && "Unreachable reached!");
  __builtin_unreachable();
}

template <typename T>
class Observer {
  T * t_;

 public:
  Observer() : t_{nullptr} {}

  Observer(T * t) : t_{t} {}

  operator bool() {
    return t_;
  }

  T operator*() {
    ABSL_ASSERT(t_ && "Null Observer observed!");
    return *t_;
  }

  T* operator->() {
    ABSL_ASSERT(t_ && "Null Observer called through!");
    return t_;
  }
};

} // namespace lox
#endif // LOX_UTILS_H_
