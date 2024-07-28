#pragma once
#include <any>
#include <cpplinq/detail/operators/operator_base.hpp>
#include <optional>

namespace cpplinq::detail::operators {

struct comparison_operator : public operator_base {
  std::string column_name;
  std::string value;
  std::optional<std::any> cached_value;
};

}  // namespace cpplinq::detail::operators