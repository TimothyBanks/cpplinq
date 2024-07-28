#include <cpplinq/detail/insert_context.hpp>
#include <cpplinq/detail/regex.hpp>

namespace cpplinq::detail {

bool is_insert_statement(const std::string& sql) {
  return cpplinq::regex::begins_with(sql, "INSERT ");
}

insert_context make_insert_context(const std::string& sql) {
  return {};
}

}  // namespace cpplinq::detail