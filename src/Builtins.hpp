#ifndef LOX_BUILTINS_HPP
#define LOX_BUILTINS_HPP

#include "Callable.hpp"
#include "Interpreter.hpp"
#include "Utils.hpp"

#include <fmt/format.h>

#include <chrono>

namespace lox {

class Now : public Callable {
 public:
  ~Now() = default;
  // Doesn't need the interpreter, and has no args
  ExprResult operator()(Interpreter&, Args&& = {}) override {
    static auto st = std::chrono::steady_clock::now();
    std::chrono::duration<double> ret = std::chrono::steady_clock::now() - st;
    return ret.count();
  }

  int arity() override { return 0; }
  std::string to_string() override { return "<fn now>"; }
};

class Print : public Callable {
 public:
  ~Print() = default;
  ExprResult operator()(Interpreter&, Args&& args = {}) override {
    for(const auto &e : args) {
      fmt::print("{}\n", lox::to_string(e));
    }
    return 1.0;
  }

  // We should be smart and make this variadic. That means abstracting the
  // int return type to something else that can indicate multivariance TODO
  int arity() override { return 1; }
  std::string to_string() override { return "<fn print>"; }
};

} // namespace lox

#endif // LOX_BUILTINS_HPP
