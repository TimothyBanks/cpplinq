#pragma once
#include <cpplinq/detail/column.hpp>
#include <cpplinq/detail/cursor.hpp>
#include <cpplinq/detail/operators/expression_tree.hpp>
#include <cpplinq/detail/range_info.hpp>
#include <optional>
#include <string_view>

namespace cpplinq::detail {

struct update_context {
  std::string table_name;
  std::string table_alias;
  operators::expression_tree et;
  std::optional<range_info> range;
  std::vector<column> values;
};

bool is_update_statement(const std::string& sql);
update_context make_update_context(const std::string& sql);

}  // namespace cpplinq::detail