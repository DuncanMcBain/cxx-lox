#ifndef LOX_ERROR_H_
#define LOX_ERROR_H_

#include <absl/strings/string_view.h>

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
};

void report_error(absl::string_view msg, const Location &loc);

} // namespace lox

#endif // LOX_ERROR_H_
