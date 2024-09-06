#pragma once
#include <cpplinq/detail/column.hpp>
#include <cpplinq/detail/cpplinq_exception.hpp>
#include <cpplinq/detail/cursor.hpp>
#include <cpplinq/detail/delete_context.hpp>
#include <cpplinq/detail/select_context.hpp>
#include <cstddef>
#include <memory>
#include <vector>

namespace cpplinq::detail::traits {

struct any_table {
 public:
 private:
  struct table_base {
    virtual const std::string& name() = 0;
    virtual const std::vector<std::string>& columns() = 0;
    virtual cpplinq::detail::cursor execute(select_context& context) const = 0;
    virtual cpplinq::detail::cursor execute(delete_context& context) const = 0;
  };

  template <typename Table_trait>
  struct table : public table_base {
    using table_trait = Table_trait;

    virtual const std::string& name() override { return table_trait::name(); };

    virtual const std::vector<std::string>& columns() override {
      return table_trait::columns();
    }

    template <typename Context>
    cpplinq::detail::cursor execute(Context& context, std::nullptr_t) const {
      auto index_name = std::string{};
      if (context.range) {
        index_name = context.range->index_name;
      }
      auto index = table_trait::index_for(index_name);
      return index.execute(context);
    }

    virtual cpplinq::detail::cursor execute(
        select_context& context) const override {
      return execute(context, nullptr);
    }

    virtual cpplinq::detail::cursor execute(
        delete_context& context) const override {
      return execute(context, nullptr);
    }
  };

  std::shared_ptr<table_base> ptr;

 public:
  any_table() = default;
  any_table(const any_table&) = default;
  any_table(any_table&&) = default;

  template <typename Table_trait>
  any_table(Table_trait) : ptr{std::make_shared<table<Table_trait>>()} {}

  any_table& operator=(const any_table&) = default;
  any_table& operator=(any_table&&) = default;

  const std::string& name() const;
  const std::vector<std::string>& columns() const;

  template <typename Context>
  cpplinq::detail::cursor execute(Context& context) const {
    if (!ptr) {
      throw cpplinq::detail::cpplinq_exception{"Invalid table state"};
    }
    return ptr->execute(context);
  }
};

}  // namespace cpplinq::detail::traits
