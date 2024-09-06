#include <cpplinq/detail/range_info.hpp>
#include <cpplinq/detail/regex.hpp>
#include <cpplinq/detail/string.hpp>

namespace cpplinq::detail {

range_info make_range_info(const std::string& s) {
  auto range = range_info{};

  auto extract = [](auto& s) {
    auto trimmed = cpplinq::detail::string::trim(s);
    trimmed.pop_back();
    trimmed = trimmed.substr(1);
    auto tokens = regex::tokenize(trimmed, ',');
    for (auto& c : tokens) {
      c = cpplinq::detail::string::trim(c);
    }
    return tokens;
  };

  auto subtokens = regex::split(s, " UPPER_BOUND ");
  if (subtokens.size() > 1) {
    // There is an upper bound clause.
    range.upper_bound = extract(subtokens.back());
  }

  subtokens = regex::split(subtokens.front(), " LOWER_BOUND ");
  if (subtokens.size() > 1) {
    // There is a lower bound clause.
    range.lower_bound = extract(subtokens.back());
  }

  range.index_name = cpplinq::detail::string::trim(subtokens.front());

  return range;
}

}  // namespace cpplinq::detail