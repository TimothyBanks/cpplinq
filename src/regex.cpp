#include <cpplinq/details/regex.hpp>
#include <regex>

namespace cpplinq::regex {

bool begins_with(const std::string &s, const std::string &token) {
  auto pattern = std::string{"^"} + token;
  auto regex_pattern = std::regex{pattern, std::regex_constants::icase};
  return std::regex_search(s, regex_pattern);
}

std::string to_regex(const std::string &sql) {
  auto expression = std::string{};
  for (auto i = size_t{0}; i < sql.size(); ++i) {
    auto ch = sql[i];
    switch (ch) {
    case '%':
      expression += ".*"; // Match any sequence of characters
      break;
    case '_':
      expression += "."; // Match any single character
      break;
    case '\\':
      expression += "\\\\"; // Escape backslash
      break;
    default:
      // Escape special regex characters
      if (std::ispunct(ch)) {
        expression += "\\";
      }
      expression += ch;
      break;
    }
  }
  return expression;
}

bool match(const std::string &s, const std::string &pattern,
           bool case_sensitive) {
  auto regex_pattern =
      std::regex{pattern, case_sensitive ? std::regex_constants::icase : -1};
  return std::regex_search(s, regex_pattern);
}

std::vector<std::string> split_(const std::string &s,
                                const std::string &token) {
  auto tokens = std::vector<std::string>{};
  auto regex_pattern = std::regex{token};
  auto iter = std::sregex_token_iterator{
      std::begin(s), std::end(s), regex_pattern, std::regex_constants::icase};
  auto end = std::sregex_token_iterator{};

  if (iter != end) {
    tokens.push_back(*iter);
    ++iter;
  }
  auto left_over = std::string{};
  if (iter != end) {
    left_over += *iter;
  }
  while (iter != end) {
    left_over += token;
    left_over += *iter;
    ++iter;
  }
  if (!left_over.empty()) {
    tokens.push_back(left_over);
  }

  return tokens;
}

std::vector<std::string> tokenize_(const std::string &s,
                                   const std::string &token) {
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

std::vector<std::string> tokenize(const std::string &s,
                                  const std::string &token) {
  static const auto escape = std::string{"\\b"};
  return tokenize_(s, escape + token + escape);
}

std::vector<std::string> tokenize(const std::string &s, char token) {
  return tokenize_(s, std::string{token});
}

} // namespace cpplinq::regex