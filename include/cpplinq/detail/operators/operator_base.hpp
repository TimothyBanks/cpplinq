#pragma once
#include <memory>
#include <string>

namespace cpplinq::detail::operators {

enum class operator_type {
  and_,
  between,
  equal_to,
  greater_than_equal,
  greater_than,
  group,
  ilike,
  in,
  is,
  less_than_equal,
  less_than,
  like,
  not_equal,
  not_,
  or_
};

struct operator_base {
  std::string original_statement;
  operator_type type;
};

using operator_ptr = std::unique_ptr<operator_base>;

}  // namespace cpplinq::detail::operators