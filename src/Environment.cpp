#include "Environment.h"

namespace lox {

void Environment::assign(Token tok, ExprResult val) {
  if (env_.find(tok.lexeme()) != env_.end()) {
    env_[tok.lexeme()] = val;
    return;
  }
  throw RuntimeError("Undefined variable: ");
}

void Environment::define(absl::string_view name, ExprResult val) {
  env_.emplace(name, val);
}

ExprResult Environment::get(Token tok) {
  return env_.find(tok.identifier()) != env_.end()
             ? env_[tok.string()]
             : throw RuntimeError("Undefined variable");
}

} // namespace lox
