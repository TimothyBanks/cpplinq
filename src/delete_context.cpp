#include <cpplinq/detail/delete_context.hpp>
#include <cpplinq/detail/regex.hpp>

namespace cpplinq::detail {

bool is_delete_statement(const std::string& sql) {
  return cpplinq::regex::begins_with(sql, "DELETE ");
}

delete_context make_delete_context(const std::string& sql) {
  return {};
}

}  // namespace cpplinq::detail