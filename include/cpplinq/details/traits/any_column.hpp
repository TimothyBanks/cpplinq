#pragma once
#include <cstddef>
#include <memory>

namespace cpplinq::details::traits {

struct any_column {
 private:
  struct column_base {
    virtual const std::string& name() = 0;
  };

  template <typename Column_trait>
  struct column : public column_base {
    using column_trait = Column_trait;
  };

  std::shared_ptr<column_base> ptr;

 public:
  any_column() = default;
  any_column(const any_column&) = default;
  any_column(any_column&&) = default;
  template <typename Column_trait>
  any_column(std::nullptr_t);

  any_column& operator=(const any_column&) = default;
  any_column& operator=(any_column&&) = default;
};

}  // namespace cpplinq::details::traits
