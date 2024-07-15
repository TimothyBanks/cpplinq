#pragma once
#include <cpplinq/details/cursor.hpp>
#include <string_view>

namespace cpplinq::details {

struct update_context {};

bool is_update_statement(const std::string& sql);
update_context make_update_context(const std::string& sql);

}  // namespace cpplinq::details