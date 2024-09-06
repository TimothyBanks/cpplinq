#pragma once
#include <cpplinq/detail/operators/expression_tree.hpp>
#include <cpplinq/detail/range_info.hpp>
#include <optional>

namespace cpplinq::detail {

struct delete_context {
  std::string table_name;
  std::string table_alias;
  operators::expression_tree et;
  std::optional<range_info> range;
  std::optional<size_t> limit;
};

bool is_delete_statement(const std::string& sql);
delete_context make_delete_context(const std::string& sql);

}  // namespace cpplinq::detail