#pragma once
#include <cstdlib>
#include <string>
#include <type_traits>

namespace cpplinq::details::traits {

template <typename T, typename Enabled = void>
struct underlying_column_type;

template <typename T>
struct underlying_column_type<
    T,
    typename std::enable_if_t<std::is_integral_v<T>>> {
  static T from_string(const std::string& s) {
    return static_cast<T>(std::atoll(s.c_str()));
  }
  static std::string to_string(const T& t) {
    using namespace std;
    return to_string(t);
  }
  static std::string to_string(const std::any& a) {
    using namespace std;
    return to_string(any_cast<const T&>(a));
  }
};

template <typename T>
struct underlying_column_type<
    T,
    typename std::enable_if_t<std::is_floating_point_v<T>>> {
  static T from_string(const std::string& s) {
    return static_cast<T>(std::atof(s.c_str()));
  }
  static std::string to_string(const T& t) {
    using namespace std;
    return to_string(t);
  }
  static std::string to_string(const std::any& a) {
    using namespace std;
    return to_string(any_cast<const T&>(a));
  }
};

template <>
struct underlying_column_type<std::string> {
  static const std::string& from_string(const std::string& s) { return s; }
  static const std::string& to_string(const std::string& t) { return t; }
  static std::string to_string(const std::any& a) {
    return std::any_cast<std::string>(a);
  }
};

// TODO:  Think about how to implement this.  (Along with non scalar types in
// general)
template <typename T>
struct underlying_column_type<std::vector<T>> {
  static std::vector<T> from_string(const std::string& s) { return {}; }
  static std::vector<T> to_string(const std::string& t) { return {}; }
  static std::string to_string(const std::any& a) { return {}; }
};
}  // namespace cpplinq::details::traits