#pragma once

namespace cpplinq::details::traits {

template <typename T>
struct underlying_column_type {
  static T from_string(const std::string& s) { return {}; }
  static std::string to_string(const T& t) { return {}; }
  static std::string to_string(const std::any& a) { return {}; }
};

}  // namespace cpplinq::details::traits