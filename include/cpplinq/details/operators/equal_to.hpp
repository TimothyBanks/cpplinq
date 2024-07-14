#pragma once
#include <cpplinq/details/operators/comparison_operator.hpp>

namespace cpplinq::details::operators {

struct equal_to : public comparison_operator {};

} // namespace cpplinq::details::operators