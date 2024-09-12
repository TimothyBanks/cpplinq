#pragma once
#include <cpplinq/detail/column.hpp>
#include <cpplinq/detail/cursor.hpp>
#include <string_view>

namespace cpplinq::detail {

struct insert_context {
  std::string table_name;
  std::string table_alias;
  std::vector<column> columns;
  std::vector<std::vector<std::string>> values;
};

bool is_insert_statement(const std::string& sql);
insert_context make_insert_context(const std::string& sql);

}  // namespace cpplinq::detail