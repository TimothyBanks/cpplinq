#include <cpplinq/detail/call_context.hpp>
#include <cpplinq/detail/delete_context.hpp>
#include <cpplinq/detail/insert_context.hpp>
#include <cpplinq/detail/procedure_registry.hpp>
#include <cpplinq/detail/select_context.hpp>
#include <cpplinq/detail/table_registry.hpp>
#include <cpplinq/detail/update_context.hpp>
#include <cpplinq/sql_context.hpp>

namespace cpplinq::sql_context {

cpplinq::detail::cursor execute(const std::string& sql) {
  if (detail::is_call_statement(sql)) {
    auto context = detail::make_call_context(sql);
    auto& procedure =
        cpplinq::detail::procedure_registry::instance().find(context.name);
    return procedure.invoke(context.arguments);
  }

  auto table_execute = [](auto&& context) {
    auto& table =
        cpplinq::detail::table_registry::instance().find(context.table_name);
    return table.execute(context);
  };

  if (detail::is_delete_statement(sql)) {
    return table_execute(detail::make_delete_context(sql));
  }
  if (detail::is_insert_statement(sql)) {
    return table_execute(detail::make_insert_context(sql));
  }
  if (detail::is_select_statement(sql)) {
    return table_execute(detail::make_select_context(sql));
  }
  if (detail::is_update_statement(sql)) {
    return table_execute(detail::make_update_context(sql));
  }
  return {};
}

}  // namespace cpplinq::sql_context