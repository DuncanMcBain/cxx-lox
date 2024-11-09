#ifndef LOX_ENVIRONMENT_HPP
#define LOX_ENVIRONMENT_HPP

#include "Expr.hpp"
#include "Token.hpp"
#include "Utils.hpp"

#include <absl/container/flat_hash_map.h>
#include <absl/container/inlined_vector.h>

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

using EnvironmentStack = absl::InlinedVector<Environment, 8>;

} // namespace lox

#endif // LOX_ENVIRONMENT_HPP
