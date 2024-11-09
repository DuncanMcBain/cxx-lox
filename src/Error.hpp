#ifndef LOX_ERROR_HPP
#define LOX_ERROR_HPP

#include <absl/strings/string_view.h>

#include <exception>
#include <string>

namespace lox {

// TODO: Rework this. Locations don't need to be made again and again.
// Have proper error class
struct Location {
  std::string where_ = "<stdin>";
  int line_          = 0;
  int chr_           = 0;
  int end_chr_       = -1;

  Location() = default;

  Location &where(absl::string_view where) {
    where_ = where;
    return *this;
  }

  Location &line(int line) {
    line_ = line;
    return *this;
  }

  Location &chr(int chr) {
    chr_ = chr;
    return *this;
  }

  Location &end_chr(int end_chr) {
    end_chr_ = end_chr;
    return *this;
  }
};

void report_error(absl::string_view msg, const Location &loc);

class ParseError : public std::exception {
  std::string msg_;
  Location loc_;

 public:
  ParseError(absl::string_view msg, Location loc)
      : msg_(msg)
      , loc_(loc) {}
  virtual const char *what() const noexcept { return msg_.c_str(); }

  Location const &location() const noexcept { return loc_; }
};

class RuntimeError : public std::exception {
  std::string msg_;

 public:
  RuntimeError(absl::string_view msg)
      : msg_(msg) {}
  virtual const char *what() const noexcept { return msg_.c_str(); }
};

} // namespace lox

#endif // LOX_ERROR_HPP
