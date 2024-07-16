#pragma once
#include <cpplinq/details/operators/operator_base.hpp>
#include <unordered_set>

namespace cpplinq::details::operators {

struct is: public operator_base {
  std::string column;
  operator_ptr op;
};

}  // namespace cpplinq::details::operators