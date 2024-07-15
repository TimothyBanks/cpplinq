#pragma once
#include <cpplinq/details/cursor.hpp>
#include <string_view>

namespace cpplinq::details {

struct call_context {};

bool is_call_statement(const std::string &sql);
call_context make_call_context(const std::string &sql);

} // namespace cpplinq::details