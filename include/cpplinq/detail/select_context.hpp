#pragma once
#include <cpplinq/detail/column.hpp>
#include <cpplinq/detail/cursor.hpp>
#include <cpplinq/detail/operators/expression_tree.hpp>
#include <optional>

namespace cpplinq::detail {

struct select_context {
  struct range_info {
    std::string index_name;
    std::optional<std::vector<std::string>> lower_bound;
    std::optional<std::vector<std::string>> upper_bound;
  };

  std::vector<column> columns;
  std::string table_name;
  std::string table_alias;
  operators::expression_tree et;
  std::optional<range_info> range;
  std::optional<size_t> limit;
  std::optional<size_t> offset;
};

bool is_select_statement(const std::string& sql);
select_context make_select_context(const std::string& sql);

}  // namespace cpplinq::detail