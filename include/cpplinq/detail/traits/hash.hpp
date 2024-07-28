#pragma once
#include <cstddef>
#include <string>

namespace cpplinq::detail::traits {

constexpr size_t hash(const char* str,
                      size_t length,
                      size_t running_hash = 2166136261u) {
  return length == 0
             ? running_hash
             : hash(str + 1, length - 1,
                    (running_hash ^ static_cast<size_t>(*str)) * 16777619u);
}

template <size_t N>
constexpr size_t hash(const char (&str)[N]) {
  return hash(str, N - 1);
}

}  // namespace cpplinq::detail::traits