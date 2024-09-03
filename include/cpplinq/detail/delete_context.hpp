#pragma once
#include <cpplinq/detail/cursor.hpp>
#include <string_view>

namespace cpplinq::detail {

struct delete_context {};

bool is_delete_statement(const std::string& sql);
delete_context make_delete_context(const std::string& sql);

}  // namespace cpplinq::detail