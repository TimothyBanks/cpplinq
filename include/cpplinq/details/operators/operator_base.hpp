#pragma once
#include <memory>
#include <string>

namespace cpplinq::details::operators {

struct operator_base {
  std::string original_statement;
};

using operator_ptr = std::unique_ptr<operator_base>;

}  // namespace cpplinq::details::operators