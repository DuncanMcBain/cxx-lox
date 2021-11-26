cmake_minimum_required(VERSION 3.10.2)

project(cxx_lox)
find_package(fmt REQUIRED)
find_package(mio REQUIRED)

add_executable(cxx_loxi
  src/lox.cpp)
target_link_libraries(cxx_loxi
  PRIVATE
    fmt::fmt
    mio::mio)
