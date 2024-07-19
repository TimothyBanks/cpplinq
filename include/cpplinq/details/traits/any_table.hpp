#pragma once
#include <cpplinq/details/column.hpp>
#include <cpplinq/details/cursor.hpp>
#include <cpplinq/details/select_context.hpp>
#include <cstddef>
#include <memory>
#include <vector>

namespace cpplinq::details::traits {

template <typename Table_trait>
cpplinq::details::cursor execute(select_context& context) {
  // TODO:  Add in the RANGE clause so the user can specify which index.
  // Currently this will iterate the entire table.
  // Here is the process I think that should happen:
  // 1.  The index_trait will refer to an index type and we will specify the
  // index_type in the DECLARE_TABLE
  // 2.  An any_index will need to be created so that I can ask the table_trait
  // for the index with the given name. 2.1 The template arguments for the
  // any_index should be a table_trait and an index trait.
  // 3.  That any_index (which will contain the index trait), will know how to
  // create the tuple from the token vector of values.
  // 4.  This iterate code will be move to the any_index code
  auto& index = Table_trait::table_type::instance().primary_index();

  auto cursor = cpplinq::details::cursor{};
  cursor.columns = context.columns;

  for (auto it = std::begin(index); it != std::end(index); ++it) {
    auto& value = *it;

    // TODO:  Aliases for columns need to be factored in during evaluation.
    if (!Table_trait::evaluate(value, context.et)) {
      continue;
    }

    cursor.results.emplace_back();
    auto& row = cursor.results.back();
    for (const auto& column : context.columns) {
      row.emplace_back(Table_trait::column_value(column.name, value));
    }
  }

  return cursor;
}

struct any_table {
 public:
 private:
  struct table_base {
    virtual const std::string& name() = 0;
    virtual const std::vector<std::string>& columns() = 0;
    virtual cpplinq::details::cursor execute(select_context& context) const = 0;
  };

  template <typename Table_trait>
  struct table : public table_base {
    using table_trait = Table_trait;

    virtual const std::string& name() override { return table_trait::name(); };

    virtual const std::vector<std::string>& columns() override {
      return table_trait::columns();
    }

    virtual cpplinq::details::cursor execute(
        select_context& context) const override {
      return cpplinq::details::traits::execute<table_trait>(context);
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

  cpplinq::details::cursor execute(select_context& context) const;
};

}  // namespace cpplinq::details::traits
