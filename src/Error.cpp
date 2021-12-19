#include <string_view>

#include <fmt/format.h>

#include "Error.h"

namespace lox {

void report_error(absl::string_view msg, const Location &loc) {
  fmt::print("Error {} in lox program at {}:{}:{}\n", msg, loc.where_,
             loc.line_, loc.chr_);
}

} // namespace lox
