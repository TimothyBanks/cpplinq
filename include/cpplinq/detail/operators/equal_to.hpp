#pragma once
#include <cpplinq/detail/operators/comparison_operator.hpp>

namespace cpplinq::detail::operators {

struct equal_to : public comparison_operator {};

}  // namespace cpplinq::detail::operators