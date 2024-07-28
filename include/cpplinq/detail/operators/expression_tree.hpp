#pragma once
#include <cpplinq/detail/operators/operator_base.hpp>

namespace cpplinq::detail::operators {

struct expression_tree {
  operator_ptr root;
};

}  // namespace cpplinq::detail::operators