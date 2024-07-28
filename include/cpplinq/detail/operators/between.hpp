#pragma once
#include <any>
#include <cpplinq/detail/operators/operator_base.hpp>
#include <optional>

namespace cpplinq::detail::operators {

struct between : public operator_base {
  std::string column;
  std::string begin;
  std::string end;
  std::optional<std::any> cached_begin;
  std::optional<std::any> cached_end;
};

}  // namespace cpplinq::detail::operators