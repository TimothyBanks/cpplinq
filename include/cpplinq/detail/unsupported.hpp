#pragma once
#include <string>

namespace cpplinq::detail {

void check_unsupported_token(const std::string& sql, const std::string& token);

}  // namespace cpplinq::detail