#pragma once
#include <any>
#include <cpplinq/details/operators/operator_base.hpp>
#include <optional>
#include <unordered_set>
#include <vector>

namespace cpplinq::details::operators {

struct in : public operator_base {
  std::string column;
  std::unordered_set<std::string> values;
  std::optional<std::vector<std::any>> cached_values;
};

} // namespace cpplinq::details::operators