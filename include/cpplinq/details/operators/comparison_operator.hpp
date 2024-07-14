#pragma once
#include <cpplinq/details/operators/operator_base.hpp>

namespace cpplinq::details::operators {

struct comparison_operator : public operator_base {
  std::string column_name;
  std::string value;
};

} // namespace cpplinq::details::operators