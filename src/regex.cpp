#include <cpplinq/details/regex.hpp>
#include <regex>

namespace cpplinq::regex {

bool begins_with(const std::string &s, const std::string &token) {
  auto pattern = std::string{"^"} + token;
  auto regex_pattern = std::regex{pattern, std::regex_constants::icase};
  return std::regex_search(s, regex_pattern);
}

std::vector<std::string> split_(const std::string &s, const std::string& token) {
  auto tokens = std::vector<std::string>{};
  auto regex_pattern = std::regex{token};
  auto iter = std::sregex_token_iterator{
      std::begin(s), std::end(s), regex_pattern, std::regex_constants::icase};
  auto end = std::sregex_token_iterator{};

  while (iter != end) {
    tokens.push_back(*iter);
    ++iter;
  }

  return tokens;
}

std::vector<std::string> split(const std::string &s, const std::string &token) {
  static const auto escape = std::string{"\\b"};
  return split_(s, escape + token + escape);
}

std::vector<std::string> split(const std::string &s, char token) {
  return split_(s, std::string{token});
}

} // namespace cpplinq::regex