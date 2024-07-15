#pragma once

namespace cpplinq::details::traits {

template <typename T>
struct underlying_column_type {
  T from_string(const std::string& s);
  std::string to_string(const T& t);
  std::string to_string(const std::any& a);
};

}  // namespace cpplinq::details::traits