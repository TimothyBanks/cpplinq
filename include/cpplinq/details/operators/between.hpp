#pragma once
#include <cpplinq/details/operators/operator_base.hpp>

namespace cpplinq::details::operators {

struct between : public operator_base {
  std::string column;
  std::string begin;
  std::string end;
};

} // namespace cpplinq::details::operators