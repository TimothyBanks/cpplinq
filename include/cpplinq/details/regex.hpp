#pragma once
#include <string>
#include <string_view>
#include <vector>

namespace cpplinq::regex {

bool begins_with(const std::string &s, const std::string &chunk);

std::vector<std::string> split(const std::string &s, const std::string &token);
std::vector<std::string> split(const std::string &s, char token);
std::vector<std::string> tokenize(const std::string &s, const std::string &token);
std::vector<std::string> tokenize(const std::string &s, char token);
bool match(const std::string& s, const std::string& pattern, bool case_sensitive = false);
std::string to_regex(const std::string& sql);

} // namespace cpplinq::regex