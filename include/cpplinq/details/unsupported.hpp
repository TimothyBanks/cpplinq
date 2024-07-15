#pragma once
#include <cpplinq/details/cpplinq_exception.hpp>
#include <cpplinq/details/regex.hpp>
#include <string>

namespace cpplinq::details {

void check_unsupported_token(const std::string& sql, const std::string& token) {
  auto tokens = regex::split(sql, " ORDER BY ");
  if (tokens.size() <= 1) {
    return;
  }
  throw cpplinq::details::cpplinq_exception{"Token " + token + " is unsupported"};
}

}  // namespace cpplinq::details