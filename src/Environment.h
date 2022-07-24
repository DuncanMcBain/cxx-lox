#ifndef LOX_ENVIRONMENT_H_
#define LOX_ENVIRONMENT_H_

#include "Expr.h"
#include "Token.h"

#include <absl/container/flat_hash_map.h>

namespace lox {

class Environment {
  using EnvMap = absl::flat_hash_map<std::string, ExprResult>;
  EnvMap env;

 public:
  void define(absl::string_view, ExprResult);

  ExprResult get(Token);
};

} // namespace lox

#endif // LOX_ENVIRONMENT_H_
