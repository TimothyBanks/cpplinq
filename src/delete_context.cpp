#include <cpplinq/details/delete_context.hpp>
#include <cpplinq/details/regex.hpp>

namespace cpplinq::details::delete_context {

bool is_this_statement(const std::string &sql) {
  return cpplinq::regex::begins_with(sql, "DELETE ");
}

cursor execute(const std::string &sql) { return {}; }

} // namespace cpplinq::details::delete_context