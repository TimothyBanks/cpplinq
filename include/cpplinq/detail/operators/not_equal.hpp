#pragma once
#include <cpplinq/detail/operators/comparison_operator.hpp>

namespace cpplinq::detail::operators {

struct not_equal : public comparison_operator {};

}  // namespace cpplinq::detail::operators