#include "Environment.h"

namespace lox {

void Environment::define(absl::string_view name, ExprResult val) {
  env.emplace(name, val);
}

ExprResult Environment::get(Token name) {
  return env.find(name.string()) != env.end()
             ? env[name.string()]
             : throw RuntimeError("Undefined variable");
}

} // namespace lox
