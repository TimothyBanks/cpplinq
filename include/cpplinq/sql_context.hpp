#pragma once
#include <cpplinq/details/cursor.hpp>
#include <string_view>

namespace cpplinq::sql_context {

cpplinq::details::cursor execute(const std::string& sql);

}