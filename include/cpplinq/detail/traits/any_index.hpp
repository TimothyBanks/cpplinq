#pragma once
#include <cpplinq/detail/aggregates/aggregate.hpp>
#include <cpplinq/detail/column.hpp>
#include <cpplinq/detail/cpplinq_exception.hpp>
#include <cpplinq/detail/cursor.hpp>
#include <cpplinq/detail/delete_context.hpp>
#include <cpplinq/detail/select_context.hpp>
#include <cstddef>
#include <limits>
#include <memory>
#include <vector>

namespace cpplinq::detail::traits {

template <typename Index_trait, typename Context>
std::pair<typename Index_trait::iterator_type,
          typename Index_trait::iterator_type>
index_range(Context& context) {
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

  return std::make_pair(std::move(begin), std::move(end));
}

template <typename Table_trait, typename Index_trait>
cpplinq::detail::cursor execute(delete_context& context) {
  auto [begin, end] = index_range<Index_trait>(context);

  auto cursor = cpplinq::detail::cursor{};
  cursor.columns = {column{.name = "rows affected"}};

  auto& index_ = Index_trait::index_type::instance();
  auto count = size_t{0};
  for (auto it = begin; it != end;) {
    auto& value = *it;

    // TODO:  Aliases for columns need to be factored in during evaluation.
    if (!Table_trait::evaluate(value, context.et)) {
      ++it;
      continue;
    }
    // TODO: This needs implemented on the index.
    // it = index_.erase(it);
    ++count;
  }

  cursor.results.emplace_back();
  cursor.results.back().emplace_back(count);
  return cursor;
}

template <typename Table_trait, typename Index_trait>
cpplinq::detail::cursor execute(select_context& context) {
  auto [begin, end] = index_range<Index_trait>(context);

  auto cursor = cpplinq::detail::cursor{};
  cursor.columns = context.columns;

  auto offset = context.offset.value_or(0);
  auto limit = context.limit.value_or(std::numeric_limits<size_t>::max());

  auto aggregates_ = std::vector<detail::aggregates::aggregate>{};
  auto has_non_aggregated_data = false;
  for (const auto& column : context.columns) {
    if (column.aggregate.empty()) {
      has_non_aggregated_data = true;
      continue;
    }
    aggregates_.push_back(detail::aggregates::make(column.aggregate));
  }

  if (has_non_aggregated_data && !aggregates_.empty()) {
    // TODO:  This is true UNTIL we support the GROUP BY clause.
    //        But even then, the non aggregated data is limited to the columns
    //        being grouped on.
    throw detail::cpplinq_exception{
        "SELECT statement can not contain both aggregated and non aggregated "
        "columns."};
  }

  std::function<void(const typename Table_trait::record_type&)> process_row =
      [&](const typename Table_trait::record_type& value) {
        cursor.results.emplace_back();
        auto& row = cursor.results.back();
        for (const auto& column : context.columns) {
          row.emplace_back(Table_trait::column_value(column.name, value));
        }
      };

  if (!has_non_aggregated_data) {
    process_row = [&](const typename Table_trait::record_type& value) {
      for (auto i = size_t{0}; i < context.columns.size(); ++i) {
        auto& column = context.columns[i];
        if (column.name == "*") {
          // In this case we just need to invoke the aggregate.  Most
          // likely this is a COUNT aggregate.
          aggregates_[i].accumulate(static_cast<int64_t>(0));
        } else {
          Table_trait::invoke(column.name, [&](const auto& column_trait) {
            aggregates_[i].accumulate(column_trait.value(value));
          });
        }
      }
    };
  }

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

    process_row(value);

    if (--limit == 0) {
      break;
    }
  }

  if (!has_non_aggregated_data) {
    // Aggregated data doesn't get added to the cursor during the processing
    // loop.  So we need to add afterwards.
    cursor.results.emplace_back();
    auto& row = cursor.results.back();
    for (const auto& aggregate : aggregates_) {
      row.emplace_back(aggregate.value());
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
    virtual cpplinq::detail::cursor execute(select_context& context) const = 0;
    virtual cpplinq::detail::cursor execute(delete_context& context) const = 0;
  };

  template <typename Table_trait, typename Index_trait>
  struct index : public index_base {
    using table_trait = Table_trait;
    using index_trait = Index_trait;

    virtual const std::string& name() override { return index_trait::name(); };

    virtual const std::vector<std::string>& columns() override {
      return index_trait::columns();
    }

    virtual cpplinq::detail::cursor execute(
        select_context& context) const override {
      return cpplinq::detail::traits::execute<table_trait, index_trait>(
          context);
    }

    virtual cpplinq::detail::cursor execute(
        delete_context& context) const override {
      return cpplinq::detail::traits::execute<table_trait, index_trait>(
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

  template <typename Context>
  cpplinq::detail::cursor execute(Context& context) const {
    if (!ptr) {
      throw cpplinq::detail::cpplinq_exception{"Invalid table state"};
    }
    return ptr->execute(context);
  }
};

}  // namespace cpplinq::detail::traits
