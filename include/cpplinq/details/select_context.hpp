#pragma once
#include <cpplinq/details/cursor.hpp>
#include <string_view>

namespace cpplinq::details::select_context {

bool is_this_statement(const std::string &sql);
cursor execute(const std::string &sql);

} // namespace cpplinq::details::select_context