#pragma once
#include <cpplinq/details/column.hpp>
#include <cpplinq/details/cursor.hpp>
#include <cpplinq/details/select_context.hpp>
#include <cstddef>
#include <limits>
#include <memory>
#include <vector>

namespace cpplinq::details::traits {

template <typename Table_trait, typename Index_trait>
cpplinq::details::cursor execute(select_context& context) {
  auto lower_bound = std::optional<typename Index_trait::tuple_type>{};
  auto upper_bound = std::optional<typename Index_trait::tuple_type>{};
  auto begin = typename Index_trait::iterator_type{};
  auto end = typename Index_trait::iterator_type{};

  auto& index_ = Index_trait::index_type::instance();
  if (!context.range ||
      (!context.range->lower_bound && !context.range->upper_bound)) {
    begin = std::begin(index_);
    end = std::end(index_);
  } else if (context.range->lower_bound && context.range->upper_bound) {
    begin =
        index_.lower_bound(Index_trait::to_tuple(*context.range->lower_bound));
    end =
        index_.lower_bound(Index_trait::to_tuple(*context.range->upper_bound));
  } else if (context.range->lower_bound) {
    begin =
        index_.lower_bound(Index_trait::to_tuple(*context.range->lower_bound));
    end = std::end(index_);
  } else if (context.range->upper_bound) {
    begin = std::begin(index_);
    end =
        index_.lower_bound(Index_trait::to_tuple(*context.range->upper_bound));
  }

  auto cursor = cpplinq::details::cursor{};
  cursor.columns = context.columns;

  auto offset = context.offset.value_or(0);
  auto limit = context.limit.value_or(std::numeric_limits<size_t>::max());

  for (auto it = begin; it != end; ++it) {
    if (offset > 0) {
      --offset;
      continue;
    }

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

    if (--limit == 0) {
      break;
    }
  }

  return cursor;
}

struct any_index {
 public:
 private:
  struct index_base {
    virtual const std::string& name() = 0;
    virtual const std::vector<std::string>& columns() = 0;
    virtual cpplinq::details::cursor execute(select_context& context) const = 0;
  };

  template <typename Table_trait, typename Index_trait>
  struct index : public index_base {
    using table_trait = Table_trait;
    using index_trait = Index_trait;

    virtual const std::string& name() override { return index_trait::name(); };

    virtual const std::vector<std::string>& columns() override {
      return index_trait::columns();
    }

    virtual cpplinq::details::cursor execute(
        select_context& context) const override {
      return cpplinq::details::traits::execute<table_trait, index_trait>(
          context);
    }
  };

  std::shared_ptr<index_base> ptr;

 public:
  any_index() = default;
  any_index(const any_index&) = default;
  any_index(any_index&&) = default;

  template <typename Table_trait, typename Index_trait>
  any_index(Table_trait, Index_trait)
      : ptr{std::make_shared<index<Table_trait, Index_trait>>()} {}

  any_index& operator=(const any_index&) = default;
  any_index& operator=(any_index&&) = default;

  const std::string& name() const;
  const std::vector<std::string>& columns() const;

  cpplinq::details::cursor execute(select_context& context) const;
};

}  // namespace cpplinq::details::traits
