#pragma once
#include <cpplinq/details/operators/operator_base.hpp>
#include <memory>

namespace cpplinq::details::operators {

struct logical_operator : public operator_base {
  operator_ptr left_operand;
  operator_ptr right_operand;
};

} // namespace cpplinq::details::operators