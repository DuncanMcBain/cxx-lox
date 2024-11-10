#include "Environment.hpp"

namespace lox {

bool Environment::assign(Token tok, ExprResult val) {
  if (auto elem = env_.find(tok.identifier()); elem != env_.end()) {
    elem->second = val;
    return true;
  }
  return false;
}

void Environment::define(absl::string_view name, ExprResult val) {
  env_.emplace(name, val);
}

std::optional<ExprResult> Environment::get(Token tok) {
  auto id = tok.identifier();
  if (auto elem = env_.find(id); elem != env_.end()) return elem->second;
  return std::nullopt;
}

} // namespace lox
