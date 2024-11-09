#ifndef LOX_CALLABLE_HPP
#define LOX_CALLABLE_HPP

#include "Expr.h"

#include <absl/container/inlined_vector.h>

namespace lox {

class Interpreter;

using Args = absl::InlinedVector<ExprResult, 4>;

class Callable {
 public:
  virtual ~Callable() {};
  virtual ExprResult operator()(Interpreter &, Args&& = {}) = 0;
  virtual int arity() = 0;
  virtual std::string to_string() = 0;
};

} // namespace lox

#endif // LOX_CALLABLE_HPP
