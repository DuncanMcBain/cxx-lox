cmake_minimum_required(VERSION 3.10.2)

add_executable(cxx_loxi
  lox.cpp
  Error.cpp
  KeywordNames.cpp
  Scanner.cpp
  TokenTypes.cpp)

set_property(TARGET cxx_loxi PROPERTY CXX_STANDARD 20)
target_link_libraries(cxx_loxi
  PRIVATE
    absl::base
    absl::strings
    fmt::fmt
    mio::mio)

