#pragma once
#include <cpplinq/details/cursor.hpp>
#include <string>

namespace cpplinq::sql_context {

cpplinq::details::cursor execute(const std::string& sql);

}