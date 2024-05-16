#include "Environment.h"

namespace lox {

void Environment::assign(Token tok, ExprResult val) {
  if (env_.find(tok.identifier()) != env_.end()) {
    env_[tok.identifier()] = val;
    return;
  }
  if (enclosing_env_) {
    enclosing_env_->assign(tok, val);
    return;
  }
  throw RuntimeError(absl::StrCat("Undefined variable: ", tok.lexeme()));
}

void Environment::define(absl::string_view name, ExprResult val) {
  env_.emplace(name, val);
}

ExprResult Environment::get(Token tok) {
  auto id = tok.identifier();
  if (env_.find(id) != env_.end())
    return env_[id];
  if (enclosing_env_)
    return enclosing_env_->get(tok);
  throw RuntimeError(absl::StrCat("Undefined variable: ", id));
}

} // namespace lox
