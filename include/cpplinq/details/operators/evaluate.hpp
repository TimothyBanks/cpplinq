#pragma once
#include <cpplinq/details/operators/expression_tree.hpp>
#include <cpplinq/details/operators/operators.hpp>
#include <cpplinq/details/regex.hpp>

namespace cpplinq::details::operators {

enum class comparison_result { less_than, greater_than, equal, not_equal };

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type &record,
              const and_operator &op) {
  return evaluate<Table_trait>(t, op.left_operand) &&
         evaluate<Table_trait>(t, op.right_operand);
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type &record,
              const between &op) {
  if (!op.cached_begin) {
    op.cached_begin = Table_trait::from_string(op.column, op.begin);
  }

  if (!op.cached_end) {
    op.cached_end = Table_trait::from_string(op.column, op.end);
  }

  auto result = Table_trait::evaluate(op.column, record, op.cached_begin);

  if (result.count(comparison_result::equal) > 0) {
    return true;
  }

  if (result.count(comparison_result::greater_than) == 0) {
    return false;
  }

  result = Table_trait::evaluate(op.column, record, op.cached_end);

  if (result.count(comparison_result::equal) > 0) {
    return true;
  }

  return result.count(comparison_result::less_than) > 0;
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type &record,
              const equal_to &op) {
  if (!op.cached_value) {
    op.cached_value = Table_trait::from_string(op.column, op.value);
  }
  return Table_trait::evaluate(op.column, record, op.cached_value)
             .count(comparison_result::equal) > 0;
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type &record,
              const expression_tree &tree) {
  return evaluate<Table_trait>(record, tree.root);
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type &record,
              const greater_than_equal &op) {
  if (!op.cached_value) {
    op.cached_value = Table_trait::from_string(op.column, op.value);
  }
  auto result = Table_trait::evaluate(op.column, record, op.cached_value);
  return result.count(comparison_result::greater_than) > 0 ||
         result.count(comparison_result::equal) > 0;
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type &record,
              const greater_than &op) {
  if (!op.cached_value) {
    op.cached_value = Table_trait::from_string(op.column, op.value);
  }
  auto result = Table_trait::evaluate(op.column, record, op.cached_value);
  return result.count(comparison_result::greater_than) > 0
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type &record,
              const group &op) {
  return evaluate<Table_trait>(record, op.root);
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type &record,
              const ilike &op) {
  if (!op.cached_value) {
    op.cached_value = regex::to_regex(op.value);
  }
  return regex::match(Table_trait::to_string(op.column, record),
                      *(op.cached_value), false);
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type &record, const in &op) {
  if (!op.cached_values) {
    auto cached_values = std::vector<std::any>{};
    for (const auto &value : op.values) {
      cached_values.push_back(Table_trait::from_string(op.column, value));
    }
    op.cached_value = std::move(cached_values);
  }
  for (const auto &value : *(op.cached_values)) {
    auto result = Table_trait::evaluate(op.column, record, value);
    if (result.count(comparison_result::equal)) {
      return true;
    }
  }
  return false;
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type &record,
              const less_than_equal &op) {
  if (!op.cached_value) {
    op.cached_value = Table_trait::from_string(op.column, op.value);
  }
  auto result = Table_trait::evaluate(op.column, record, op.cached_value);
  return result.count(comparison_result::less_than) > 0 ||
         result.count(comparison_result::equal) > 0;
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type &record,
              const less_than &op) {
  if (!op.cached_value) {
    op.cached_value = Table_trait::from_string(op.column, op.value);
  }
  auto result = Table_trait::evaluate(op.column, record, op.cached_value);
  return result.count(comparison_result::less_than) > 0;
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type &record, const like &op) {
  if (!op.cached_value) {
    op.cached_value = regex::to_regex(op.value);
  }
  return regex::match(Table_trait::to_string(op.column, record),
                      *(op.cached_value), true);
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type &record,
              const not_equal &op) {
  if (!op.cached_value) {
    op.cached_value = Table_trait::from_string(op.column, op.value);
  }
  auto result = Table_trait::evaluate(op.column, record, op.cached_value);
  return result.count(comparison_result::not_equal) > 0;
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type &record,
              const not_operator &op) {
  return !(evaluate<Table_trait>(record, op.op));
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type &record,
              const or_operator &op) {
  return evaluate<Table_trait>(record, op.left_operand) ||
         evaluate<Table_trait>(record, op.right_operand);
}

} // namespace cpplinq::details::operators