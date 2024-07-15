#pragma once
#include <cpplinq/details/column.hpp>
#include <cpplinq/details/cursor.hpp>
#include <cpplinq/details/operators/expression_tree.hpp>
#include <optional>

namespace cpplinq::details {

struct select_context {
    std::vector<column> columns;
    std::string table_name;
    std::string table_alias;
    operators::expression_tree et;
    std::optional<size_t> limit;
    std::optional<size_t> offset;
};

bool is_select_statement(const std::string& sql);
select_context make_select_context(const std::string& sql);

} // namespace cpplinq::details