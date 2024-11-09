#include "Environment.hpp"
#include "Function.hpp"
#include "Interpreter.hpp"

namespace lox {

ExprResult Function::operator()(Interpreter &interp, Args &&args) {
  Environment function_env;
  for (int i = 0; i < decl_.tokens_.size(); i++)
    function_env.define(decl_.tokens_[i].lexeme(), args[i]);
  interp.executeBlock(decl_.statements_, std::move(function_env));
  return nullptr;
}

} // namespace lox
