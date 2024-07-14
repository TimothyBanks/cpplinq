#pragma once
#include <cpplinq/details/operators/operator_base.hpp>

namespace cpplinq::details::operators {

struct expression_tree {
  operator_ptr root;
};

} // namespace cpplinq::details::operators