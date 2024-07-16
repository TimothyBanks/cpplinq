#pragma once
#include <string>

namespace cpplinq::details {

void check_unsupported_token(const std::string& sql, const std::string& token);

}  // namespace cpplinq::details