#include <cpplinq/detail/regex.hpp>
#include <cpplinq/detail/update_context.hpp>

namespace cpplinq::detail {

bool is_update_statement(const std::string& sql) {
  return cpplinq::regex::begins_with(sql, "UPDATE ");
}

update_context make_update_context(const std::string& sql) {
  return {};
}

}  // namespace cpplinq::detail