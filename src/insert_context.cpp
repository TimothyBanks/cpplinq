#include <cpplinq/details/insert_context.hpp>
#include <cpplinq/details/regex.hpp>

namespace cpplinq::details {

bool is_insert_statement(const std::string &sql) {
  return cpplinq::regex::begins_with(sql, "INSERT ");
}

insert_context make_insert_context(const std::string &sql) { return {}; }

} // namespace cpplinq::details