#pragma once
#include <cstddef>
#include <string>

namespace cpplinq::details::traits {

constexpr size_t hash(const char* str, size_t hash_value = 0xcbf29ce484222325);
constexpr size_t hash(const std::string& str);

}  // namespace cpplinq::details::traits