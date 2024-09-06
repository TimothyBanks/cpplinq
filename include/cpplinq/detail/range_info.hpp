#pragma once
#include <optional>
#include <string>
#include <vector>

namespace cpplinq::detail {

struct range_info {
  std::string index_name;
  std::optional<std::vector<std::string>> lower_bound;
  std::optional<std::vector<std::string>> upper_bound;
};

range_info make_range_info(const std::string& s);

}  // namespace cpplinq::detail