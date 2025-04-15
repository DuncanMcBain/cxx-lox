#ifndef LOX_UTILS_HPP
#define LOX_UTILS_HPP

#include "Callable.hpp"
#include "Expr.hpp"

#include <absl/base/macros.h>

#include <cstddef>
#include <string>

namespace util {

// Deduction guides to use with variant/visit
template <typename... Ts>
struct Overloaded : public Ts... {
  using Ts::operator()...;
};
template <typename... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

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

} // namespace util

namespace lox {

class Callable;
using CallablePtr = std::shared_ptr<Callable>;

// Utils now needs to know this but not all the rest of Expr
using ExprResult =
    std::variant<bool, double, std::string, std::nullptr_t, CallablePtr>;

inline std::string to_string(ExprResult res) {
  auto visitor = util::Overloaded{
      [](bool a) -> std::string { return a ? "true" : "false"; },
      [](double a) { return std::to_string(a); },
      [](std::string a) { return a; },
      [](std::nullptr_t) -> std::string { return "nil"; },
      [](CallablePtr c) -> std::string { return c->to_string(); }
      };
  return std::visit(visitor, res);
}

} // namespace lox
#endif // LOX_UTILS_HPP
