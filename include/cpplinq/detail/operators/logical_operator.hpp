#pragma once
#include <cpplinq/detail/operators/operator_base.hpp>
#include <memory>

namespace cpplinq::detail::operators {

struct logical_operator : public operator_base {
  operator_ptr left_operand;
  operator_ptr right_operand;
};

}  // namespace cpplinq::detail::operators