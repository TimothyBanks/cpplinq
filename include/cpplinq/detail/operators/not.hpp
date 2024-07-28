#pragma once
#include <cpplinq/detail/operators/operator_base.hpp>

namespace cpplinq::detail::operators {

struct not_operator : public operator_base {
  operator_ptr op;
};

}  // namespace cpplinq::detail::operators