#pragma once
#include <any>
#include <cpplinq/detail/column.hpp>
#include <vector>

namespace cpplinq::detail {

struct cursor {
  std::vector<column> columns;
  std::vector<std::vector<std::any>> results;
};

}  // namespace cpplinq::detail