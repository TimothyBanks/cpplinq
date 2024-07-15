#include <cpplinq/details/call_context.hpp>
#include <cpplinq/details/regex.hpp>

namespace cpplinq::details {

bool is_call_statement(const std::string &sql) {
  return cpplinq::regex::begins_with(sql, "CALL ");
}

call_context make_call_context(const std::string &sql) { return {}; }

} // namespace cpplinq::details