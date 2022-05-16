#include "Error.h"
#include "Interpreter.h"
#include "Parser.h"
#include "Scanner.h"

#include <absl/strings/string_view.h>

#include <fmt/core.h>

#include <mio/mmap.hpp>

// TODO: replace this?
#include <iostream>
#include <string>

#include <sysexits.h>

std::error_code run(std::string &&);
std::error_code run_file(absl::string_view, lox::Location &);
std::error_code run_prompt(lox::Location &);

int main(int argc, char *argv[]) {
  // TODO: change to absl StatusOr?
  std::error_code err;
  lox::Location loc;
  if (argc > 2) {
    fmt::print("Usage: {} [file]\n", argv[0]);
    return EX_USAGE;
  } else if (argc == 2) {
    err = run_file(argv[1], loc);
  } else {
    err = run_prompt(loc);
  }
  if (err) {
    lox::report_error("", loc);
    return EX_DATAERR;
  }
  return EX_OK;
}

std::error_code run_file(absl::string_view file_name, lox::Location &loc) {
  std::error_code err;
  loc.where(file_name);
  auto source = mio::make_mmap_source(file_name, err);
  if (err) { return err; }
  std::string src(source.begin(), source.end());
  return run(std::move(src));
}

std::error_code run_prompt(lox::Location &loc) {
  std::string line(80u, '\0');
  do {
    fmt::print("> ");
    std::getline(std::cin, line);
    if (line.empty()) { break; }
    if (run(std::move(line))) {
      lox::report_error("Something blew up", lox::Location{});
    }
  } while (std::cin.good());
  return std::error_code{};
}

std::error_code run(std::string &&src) {
  lox::Scanner scan(src);
  auto tokens = scan.tokenise();
  for (auto t : tokens) { fmt::print("token: {}\n", std::string(t)); }
  lox::Parser p(std::move(tokens));
  auto tree = p.parse();
  static lox::Interpreter interpreter;
  interpreter.interpret(tree);
  return std::error_code{};
}
