#pragma once
#include <cpplinq/detail/operators/operator_base.hpp>

namespace cpplinq::detail::operators {

struct group : public operator_base {
  operator_ptr root;
};

}  // namespace cpplinq::detail::operators