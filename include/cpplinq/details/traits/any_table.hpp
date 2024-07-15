#pragma once
#include <cpplinq/details/column.hpp>
#include <cpplinq/details/cursor.hpp>
#include <cpplinq/details/select_context.hpp>
#include <cstddef>
#include <memory>
#include <vector>

namespace cpplinq::details::traits {

// template <typename Table_trait>
// cpplinq::details::cursor execute(const select_context &context) {
//   auto &primary_index = Table_trait::table_type::instance().primary_index();

//   for (auto it = std::begin(primary_index); it != std::end(primary_index);
//        ++it) {
//   }
// }

struct any_table {
 public:
 private:
  struct table_base {
    virtual const std::string& name() = 0;
    virtual const std::vector<std::string>& columns() = 0;
    virtual cpplinq::details::cursor execute(
        const select_context& context) const = 0;
  };

  template <typename Table_trait>
  struct table : public table_base {
    using table_trait = Table_trait;

    virtual const std::string& name() override { return table_trait::name(); };

    virtual const std::vector<std::string>& columns() override {
      return table_trait::columns();
    }

    virtual cpplinq::details::cursor execute(
        const select_context& context) const override {
      return {};
    }
  };

  std::shared_ptr<table_base> ptr;

 public:
  any_table() = default;
  any_table(const any_table&) = default;
  any_table(any_table&&) = default;

  template <typename Table_trait>
  any_table(std::nullptr_t);

  any_table& operator=(const any_table&) = default;
  any_table& operator=(any_table&&) = default;

  const std::string& name() const;
  const std::vector<std::string>& columns() const;

  cpplinq::details::cursor execute(const select_context& context) const;
};

}  // namespace cpplinq::details::traits
