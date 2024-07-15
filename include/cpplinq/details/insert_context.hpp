#pragma once
#include <cpplinq/details/cursor.hpp>
#include <string_view>

namespace cpplinq::details {

struct insert_context {};

bool is_insert_statement(const std::string &sql);
insert_context make_insert_context(const std::string &sql);

} // namespace cpplinq::details