#ifndef LOX_ENVIRONMENT_H_
#define LOX_ENVIRONMENT_H_

#include "Expr.h"
#include "Token.h"
#include "Utils.h"

#include <absl/container/flat_hash_map.h>

namespace lox {

// This looks strange but Environments can point to other Environments
class Environment;

class Environment {
  using EnvMap = absl::flat_hash_map<std::string, ExprResult>;
  Observer<Environment> enclosing_env_;
  EnvMap env_;

 public:
  Environment() = default;
  Environment(Environment* env) : enclosing_env_{env}, env_{} {}
  void assign(Token, ExprResult);
  void define(absl::string_view, ExprResult);

  ExprResult get(Token);
};

} // namespace lox

#endif // LOX_ENVIRONMENT_H_
