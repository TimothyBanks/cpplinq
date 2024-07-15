#pragma once
#include <cstddef>
#include <memory>

namespace cpplinq::details::traits {

struct any_index {
 private:
  struct index_base {};

  template <typename Index_trait>
  struct index : public index_base {
    using index_trait = Index_trait;
  };

  std::shared_ptr<index_base> ptr;

 public:
  any_index() = default;
  any_index(const any_index&) = default;
  any_index(any_index&&) = default;
  template <typename Column_trait>
  any_index(std::nullptr_t);

  any_index& operator=(const any_index&) = default;
  any_index& operator=(any_index&&) = default;
};

}  // namespace cpplinq::details::traits