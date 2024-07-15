#pragma once
#include <any>
#include <cpplinq/details/column.hpp>
#include <vector>

namespace cpplinq::details {

struct cursor {
  std::vector<column> columns;
  std::vector<std::vector<std::any>> results;
};

}  // namespace cpplinq::details