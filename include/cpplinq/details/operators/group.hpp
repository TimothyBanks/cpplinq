#pragma once
#include <cpplinq/details/operators/operator_base.hpp>

namespace cpplinq::details::operators {

struct group : public operator_base {
  operator_ptr root;
};

}  // namespace cpplinq::details::operators