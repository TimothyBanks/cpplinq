#pragma once
#include <cpplinq/detail/cursor.hpp>
#include <string_view>

namespace cpplinq::detail {

struct update_context {};

bool is_update_statement(const std::string& sql);
update_context make_update_context(const std::string& sql);

}  // namespace cpplinq::detail