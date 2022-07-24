cmake_minimum_required(VERSION 3.10.2)

project(cxx_lox)
find_package(absl REQUIRED)
find_package(fmt REQUIRED)

add_subdirectory(src)
