#include <cpplinq/details/string.hpp>
#include <unordered_set>

namespace cpplinq::details::string {

std::string& trim(std::string& s) {
  static auto chars_to_trim = std::unordered_set<char>{' ', '\'', '\"', '\0'};
  while (chars_to_trim.count(s.back()) > 0) {
    s.pop_back();
  }
  auto count = size_t{0};
  for (auto i = size_t{0}; i < s.size(); ++i) {
    if (chars_to_trim.count(s[i]) == 0) {
      break;
    }
    ++count;
  }
  s = s.substr(count);
  return s;
}

}  // namespace cpplinq::details::string