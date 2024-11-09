#ifndef LOX_FUNCTION_HPP
#define LOX_FUNCTION_HPP

#include "Callable.hpp"
#include "Expr.h"
#include "Stmt.h"

#include <memory>

namespace lox {

class Interpreter;

class Function : public Callable {
  // This is extrememly sketchy. It's bad because it is a raw reference into
  // the shared_ptr AST. We can't construct a new shared_ptr to it because this
  // would mean there were two owning pointers for this node. The visitor was
  // really only supposed to watch each node as we visited, do something then
  // leave. However this will be inserted into the environment map which must
  // keep it alive for the duration of that environment. This should imply that
  // the owership will work out because the environment can only last as long
  // as the global program scope and in that case, will live as long as the
  // AST does, but it's still very bad. The visitor might need to visit the
  // actual shared pointer which will add some bookkeeping to each funciton
  // call but would clear up this particular nastiness.
  // TODO: uh, all that
  const Fn& decl_;

 public:
  Function(const Fn &decl) : decl_(decl) {}
  ~Function() override = default;
  ExprResult operator()(Interpreter &, Args&& = {}) override;

  int arity() override { return decl_.tokens_.size(); }
  std::string to_string() override { return absl::StrCat("<fn ", decl_.name_.lexeme(), ">"); }
};

} // namespace lox

#endif // LOX_FUNCTION_HPP
