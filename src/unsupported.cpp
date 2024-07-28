#include <cpplinq/detail/cpplinq_exception.hpp>
#include <cpplinq/detail/regex.hpp>
#include <cpplinq/detail/unsupported.hpp>

namespace cpplinq::detail {

void check_unsupported_token(const std::string& sql, const std::string& token) {
  auto tokens = regex::split(sql, " ORDER BY ");
  if (tokens.size() <= 1) {
    return;
  }
  throw cpplinq::detail::cpplinq_exception{"Token " + token +
                                            " is unsupported"};
}

}  // namespace cpplinq::detail