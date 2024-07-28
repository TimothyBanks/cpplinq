#pragma once
#include <cpplinq/detail/cursor.hpp>
#include <string>

namespace cpplinq::sql_context {

cpplinq::detail::cursor execute(const std::string& sql);

}