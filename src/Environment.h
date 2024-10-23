#ifndef LOX_ENVIRONMENT_H_
#define LOX_ENVIRONMENT_H_

#include "Expr.h"
#include "Token.h"
#include "Utils.h"

#include <absl/container/flat_hash_map.h>

#include <optional>

namespace lox {

class Environment {
  using EnvMap = absl::flat_hash_map<std::string, ExprResult>;
  EnvMap env_;

 public:
  bool assign(Token, ExprResult);
  void define(absl::string_view, ExprResult);
  std::optional<ExprResult> get(Token);
};

} // namespace lox

#endif // LOX_ENVIRONMENT_H_
