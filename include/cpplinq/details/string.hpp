#pragma once
#include <string>

namespace cpplinq::details::string {

std::string& trim(std::string& s) {
  while (s.back() == ' ') {
    s.pop_back();
  }
  auto count = size_t{0};
  for (auto i = size_t{0}; i < s.size(); ++i) {
    if (s[i] != ' ') {
      break;
    }
    ++count;
  }
  s = s.substr(count);
  return s;
}

}  // namespace cpplinq::details::string