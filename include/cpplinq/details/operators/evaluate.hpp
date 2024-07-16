#pragma once
#include <cpplinq/details/operators/expression_tree.hpp>
#include <cpplinq/details/operators/operators.hpp>
#include <cpplinq/details/regex.hpp>

namespace cpplinq::details::operators {

enum class comparison_result { less_than, greater_than, equal, not_equal };

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              and_operator& op);

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              between& op);

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              equal_to& op);

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              expression_tree& tree);

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              greater_than_equal& op);

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              greater_than& op);

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record, group& op);

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record, ilike& op);

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record, in& op);

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              less_than_equal& op);

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              less_than& op);

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record, like& op);

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              not_equal& op);

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              not_operator& op);

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              or_operator& op);

template <typename T, typename U = T>
std::vector<cpplinq::details::operators::comparison_result> evaluate(
    const T& left,
    const U& right);

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              and_operator& op) {
  return evaluate<Table_trait>(record, op.left_operand) &&
         evaluate<Table_trait>(record, op.right_operand);
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              between& op) {
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
bool evaluate(const typename Table_trait::record_type& record,
              equal_to& op) {
  if (!op.cached_value) {
    op.cached_value = Table_trait::from_string(op.column_name, op.value);
  }
  return Table_trait::evaluate(op.column_name, record, op.cached_value)
             .count(comparison_result::equal) > 0;
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              expression_tree& tree) {
  return evaluate<Table_trait>(record, tree.root);
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              greater_than_equal& op) {
  if (!op.cached_value) {
    op.cached_value = Table_trait::from_string(op.column_name, op.value);
  }
  auto result = Table_trait::evaluate(op.column_name, record, op.cached_value);
  return result.count(comparison_result::greater_than) > 0 ||
         result.count(comparison_result::equal) > 0;
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              greater_than& op) {
  if (!op.cached_value) {
    op.cached_value = Table_trait::from_string(op.column_name, op.value);
  }
  auto result = Table_trait::evaluate(op.column_name, record, op.cached_value);
  return result.count(comparison_result::greater_than) > 0;
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              group& op) {
  return evaluate<Table_trait>(record, op.root);
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              ilike& op) {
  if (!op.cached_value) {
    op.cached_value = regex::to_regex(op.value);
  }
  return regex::match(Table_trait::to_string(op.column_name, record),
                      *(op.cached_value), false);
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record, in& op) {
  if (!op.cached_values) {
    auto cached_values = std::vector<std::any>{};
    for (const auto& value : op.values) {
      cached_values.push_back(Table_trait::from_string(op.column, value));
    }
    op.cached_values = std::move(cached_values);
  }
  for (const auto& value : *(op.cached_values)) {
    auto result = Table_trait::evaluate(op.column, record, value);
    if (result.count(comparison_result::equal)) {
      return true;
    }
  }
  return false;
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              less_than_equal& op) {
  if (!op.cached_value) {
    op.cached_value = Table_trait::from_string(op.column_name, op.value);
  }
  auto result = Table_trait::evaluate(op.column_name, record, op.cached_value);
  return result.count(comparison_result::less_than) > 0 ||
         result.count(comparison_result::equal) > 0;
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              less_than& op) {
  if (!op.cached_value) {
    op.cached_value = Table_trait::from_string(op.column_name, op.value);
  }
  auto result = Table_trait::evaluate(op.column_name, record, op.cached_value);
  return result.count(comparison_result::less_than) > 0;
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record, like& op) {
  if (!op.cached_value) {
    op.cached_value = regex::to_regex(op.value);
  }
  return regex::match(Table_trait::to_string(op.column_name, record),
                      *(op.cached_value), true);
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              not_equal& op) {
  if (!op.cached_value) {
    op.cached_value = Table_trait::from_string(op.column_name, op.value);
  }
  auto result = Table_trait::evaluate(op.column_name, record, op.cached_value);
  return result.count(comparison_result::not_equal) > 0;
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              not_operator& op) {
  return !(evaluate<Table_trait>(record, op.op));
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              or_operator& op) {
  return evaluate<Table_trait>(record, op.left_operand) ||
         evaluate<Table_trait>(record, op.right_operand);
}

template <typename T, typename U>
std::vector<cpplinq::details::operators::comparison_result> evaluate(
    const T& left,
    const U& right) {
  auto result = std::vector<cpplinq::details::operators::comparison_result>{};
  if (left < right) {
    result.emplace_back(cpplinq::details::operators::comparison_result::less_than);
  } else if (left > right) {
    result.emplace_back(
        cpplinq::details::operators::comparison_result::greater_than);
  }
  if (left == right) {
    result.emplace_back(cpplinq::details::operators::comparison_result::equal);
  } else {
    result.emplace_back(cpplinq::details::operators::comparison_result::not_equal);
  }
  return result;
}

}  // namespace cpplinq::details::operators