#include <cpplinq/details/delete_context.hpp>
#include <cpplinq/details/regex.hpp>

namespace cpplinq::details {

bool is_delete_statement(const std::string& sql) {
  return cpplinq::regex::begins_with(sql, "DELETE ");
}

delete_context make_delete_context(const std::string& sql) {
  return {};
}

}  // namespace cpplinq::details