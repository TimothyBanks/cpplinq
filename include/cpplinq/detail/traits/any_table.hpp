#pragma once
#include <cpplinq/detail/insert_context.hpp>
#include <cpplinq/detail/traits/any_index.hpp>
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
    virtual cpplinq::detail::cursor execute(update_context& context) const = 0;
    virtual cpplinq::detail::cursor execute(insert_context& context) const = 0;
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

    virtual cpplinq::detail::cursor execute(
        update_context& context) const override {
      return execute(context, nullptr);
    }

    virtual cpplinq::detail::cursor execute(
        insert_context& context) const override {
      for (const auto& value : context.values) {
        auto record = typename table_trait::record_type{};
        for (auto i = size_t{0}; i < value.size(); ++i) {
          table_trait::invoke(context.columns[i].name, [&](auto& ct) {
            ct.set_value(record, value[i]);
          });
        }
        table_trait::table_type::instance().push_back(std::move(record));
      }
      return make_cursor(context.values.size());
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
