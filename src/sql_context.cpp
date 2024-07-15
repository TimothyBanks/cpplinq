#include <cpplinq/details/call_context.hpp>
#include <cpplinq/details/delete_context.hpp>
#include <cpplinq/details/insert_context.hpp>
#include <cpplinq/details/select_context.hpp>
#include <cpplinq/details/update_context.hpp>
#include <cpplinq/sql_context.hpp>

namespace cpplinq::sql_context {

cpplinq::details::cursor execute(const std::string& sql) {
  if (details::is_call_statement(sql)) {
    auto context = details::make_call_context(sql);
  }
  if (details::is_delete_statement(sql)) {
    auto context = details::make_delete_context(sql);
  }
  if (details::is_insert_statement(sql)) {
    auto context = details::make_insert_context(sql);
  }
  if (details::is_select_statement(sql)) {
    auto context = details::make_select_context(sql);
  }
  if (details::is_update_statement(sql)) {
    auto context = details::make_update_context(sql);
  }
  return {};
}

}  // namespace cpplinq::sql_context