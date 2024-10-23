#include "Environment.h"

namespace lox {

bool Environment::assign(Token tok, ExprResult val) {
  if (env_.find(tok.identifier()) != env_.end()) {
    env_[tok.identifier()] = val;
    return true;
  }
  return false;
}

void Environment::define(absl::string_view name, ExprResult val) {
  env_.emplace(name, val);
}

std::optional<ExprResult> Environment::get(Token tok) {
  auto id = tok.identifier();
  if (env_.find(id) != env_.end()) return env_[id];
  return std::nullopt;
}

} // namespace lox
