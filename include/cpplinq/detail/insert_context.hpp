#pragma once
#include <cpplinq/detail/cursor.hpp>
#include <string_view>

namespace cpplinq::detail {

struct insert_context {};

bool is_insert_statement(const std::string& sql);
insert_context make_insert_context(const std::string& sql);

}  // namespace cpplinq::detail