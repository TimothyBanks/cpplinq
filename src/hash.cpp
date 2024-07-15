#include <cpplinq/details/traits/hash.hpp>

namespace cpplinq::details::traits {
constexpr size_t hash(const char* str, size_t hash_value) {
  return (*str == '\0')
             ? hash_value
             : hash(str + 1,
                    (hash_value ^ static_cast<size_t>(*str)) * 0x100000001b3);
}

constexpr size_t hash(const std::string& str) {
  return hash(str.c_str());
}
}  // namespace cpplinq::details::traits