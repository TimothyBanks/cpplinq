#pragma once
#include <cpplinq/details/operators/comparison_operator.hpp>

namespace cpplinq::details::operators {

struct not_equal : public comparison_operator {};

} // namespace cpplinq::details::operators