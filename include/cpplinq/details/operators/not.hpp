#pragma once
#include <cpplinq/details/operators/operator_base.hpp>

namespace cpplinq::details::operators {

struct not_operator : public operator_base {
  operator_ptr op;
};

} // namespace cpplinq::details::operators