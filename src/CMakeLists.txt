cmake_minimum_required(VERSION 3.10.2)

add_executable(cxx_loxi
  lox.cpp
  Environment.cpp
  Error.cpp
  Function.cpp
  Interpreter.cpp
  KeywordNames.cpp
  Parser.cpp
  Scanner.cpp
  TokenTypes.cpp)

target_compile_options(cxx_loxi PRIVATE -fdiagnostics-color=always)

set_property(TARGET cxx_loxi PROPERTY CXX_STANDARD 20)
target_link_libraries(cxx_loxi
  PRIVATE
    absl::base
    absl::flat_hash_map
    absl::inlined_vector
    absl::strings
    fmt::fmt)

