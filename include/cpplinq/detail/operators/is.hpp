#pragma once
#include <cpplinq/detail/operators/operator_base.hpp>
#include <unordered_set>

namespace cpplinq::detail::operators {

struct is : public operator_base {
  std::string column;
  operator_ptr op;
};

}  // namespace cpplinq::detail::operators