#include <fmt/core.h>

#include <mio/mmap.hpp>

#include <iostream>
#include <string>
#include <string_view>

#include <sysexits.h>

template <typename Iterator>
std::error_code run(Iterator, Iterator);
std::error_code run_file(std::string_view);
std::error_code run_prompt();

int main(int argc, char *argv[]) {
  std::error_code err;
  if (argc > 2) {
    fmt::print("Usage: cxx_loxi [file]\n");
    return EX_USAGE;
  } else if (argc == 2) {
    err = run_file(argv[1]);
  } else {
    err = run_prompt();
  }
  if (err) {
    fmt::print("fail: {}", err.message());
    return EX_DATAERR;
  }
  return EX_OK;
}

std::error_code run_file(std::string_view file_name) {
  std::error_code err;
  auto source = mio::make_mmap_source(file_name, err);
  if (err) { return err; }
  return run(source.begin(), source.end());
}

std::error_code run_prompt() {
  std::string line(80u, '\0');
  do {
    fmt::print("> ");
    std::getline(std::cin, line);
    if (line.empty()) { break; }
    run(line.begin(), line.end());
  } while (std::cin.good());
  return std::error_code{};
}

template <typename Iterator>
std::error_code run(Iterator b, Iterator e) {
  lox::Scanner scan(b, e);
  auto tokens = scan.tokenise();
  return std::error_code{};
}
