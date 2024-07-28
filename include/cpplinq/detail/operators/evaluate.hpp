#pragma once
#include <cpplinq/detail/operators/expression_tree.hpp>
#include <cpplinq/detail/operators/operators.hpp>
#include <cpplinq/detail/regex.hpp>
#include <unordered_set>

namespace cpplinq::detail::operators {

enum class comparison_result { less_than, greater_than, equal, not_equal };

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              operator_ptr& op);

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              and_operator& op);

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record, between& op);

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record, equal_to& op);

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
bool evaluate(const typename Table_trait::record_type& record, less_than& op);

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record, like& op);

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record, not_equal& op);

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              not_operator& op);

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record, or_operator& op);

template <typename T>
std::unordered_set<cpplinq::detail::operators::comparison_result> evaluate(
    const T& left,
    const T& right);

template <typename T, typename U>
std::unordered_set<cpplinq::detail::operators::comparison_result> evaluate(
    const T& left,
    const U& right);

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              operator_ptr& op) {
  switch (op->type) {
    case cpplinq::detail::operators::operator_type::and_:
      return evaluate<Table_trait>(record, static_cast<and_operator&>(*op));
    case cpplinq::detail::operators::operator_type::between:
      return evaluate<Table_trait>(record, static_cast<between&>(*op));
    case cpplinq::detail::operators::operator_type::equal_to:
      return evaluate<Table_trait>(record, static_cast<equal_to&>(*op));
    case cpplinq::detail::operators::operator_type::greater_than_equal:
      return evaluate<Table_trait>(record,
                                   static_cast<greater_than_equal&>(*op));
    case cpplinq::detail::operators::operator_type::greater_than:
      return evaluate<Table_trait>(record, static_cast<greater_than&>(*op));
    case cpplinq::detail::operators::operator_type::group:
      return evaluate<Table_trait>(record, static_cast<group&>(*op));
    case cpplinq::detail::operators::operator_type::ilike:
      return evaluate<Table_trait>(record, static_cast<ilike&>(*op));
    case cpplinq::detail::operators::operator_type::in:
      return evaluate<Table_trait>(record, static_cast<in&>(*op));
    case cpplinq::detail::operators::operator_type::is:
      return false;
    //   return evaluate<Table_trait>(record, static_cast<is&>(*op));
    case cpplinq::detail::operators::operator_type::less_than_equal:
      return evaluate<Table_trait>(record, static_cast<less_than_equal&>(*op));
    case cpplinq::detail::operators::operator_type::less_than:
      return evaluate<Table_trait>(record, static_cast<less_than&>(*op));
    case cpplinq::detail::operators::operator_type::like:
      return evaluate<Table_trait>(record, static_cast<like&>(*op));
    case cpplinq::detail::operators::operator_type::not_equal:
      return evaluate<Table_trait>(record, static_cast<not_equal&>(*op));
    case cpplinq::detail::operators::operator_type::not_:
      return evaluate<Table_trait>(record, static_cast<not_operator&>(*op));
    case cpplinq::detail::operators::operator_type::or_:
      return evaluate<Table_trait>(record, static_cast<or_operator&>(*op));
  }
  return false;
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              and_operator& op) {
  return evaluate<Table_trait>(record, op.left_operand) &&
         evaluate<Table_trait>(record, op.right_operand);
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record, between& op) {
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
bool evaluate(const typename Table_trait::record_type& record, equal_to& op) {
  if (!op.cached_value) {
    op.cached_value = Table_trait::from_string(op.column_name, op.value);
  }
  return Table_trait::evaluate(op.column_name, record, *op.cached_value)
             .count(comparison_result::equal) > 0;
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              expression_tree& tree) {
  if (!tree.root) {
    // This indicates no WHERE clause.
    return true;
  }
  return evaluate<Table_trait>(record, tree.root);
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              greater_than_equal& op) {
  if (!op.cached_value) {
    op.cached_value = Table_trait::from_string(op.column_name, op.value);
  }
  auto result = Table_trait::evaluate(op.column_name, record, *op.cached_value);
  return result.count(comparison_result::greater_than) > 0 ||
         result.count(comparison_result::equal) > 0;
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record,
              greater_than& op) {
  if (!op.cached_value) {
    op.cached_value = Table_trait::from_string(op.column_name, op.value);
  }
  auto result = Table_trait::evaluate(op.column_name, record, *op.cached_value);
  return result.count(comparison_result::greater_than) > 0;
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record, group& op) {
  return evaluate<Table_trait>(record, op.root);
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record, ilike& op) {
  if (!op.cached_value) {
    op.cached_value = regex::to_regex(op.value);
  }
  return regex::match(Table_trait::to_string(op.column_name, record),
                      std::any_cast<std::string&>(*(op.cached_value)), false);
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
  auto result = Table_trait::evaluate(op.column_name, record, *op.cached_value);
  return result.count(comparison_result::less_than) > 0 ||
         result.count(comparison_result::equal) > 0;
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record, less_than& op) {
  if (!op.cached_value) {
    op.cached_value = Table_trait::from_string(op.column_name, op.value);
  }
  auto result = Table_trait::evaluate(op.column_name, record, *op.cached_value);
  return result.count(comparison_result::less_than) > 0;
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record, like& op) {
  if (!op.cached_value) {
    op.cached_value = regex::to_regex(op.value);
  }
  return regex::match(Table_trait::to_string(op.column_name, record),
                      std::any_cast<std::string&>(*(op.cached_value)), true);
}

template <typename Table_trait>
bool evaluate(const typename Table_trait::record_type& record, not_equal& op) {
  if (!op.cached_value) {
    op.cached_value = Table_trait::from_string(op.column_name, op.value);
  }
  auto result = Table_trait::evaluate(op.column_name, record, *op.cached_value);
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

template <typename T>
std::unordered_set<cpplinq::detail::operators::comparison_result> evaluate(
    const T& left,
    const T& right) {
  auto result =
      std::unordered_set<cpplinq::detail::operators::comparison_result>{};
  if (left < right) {
    result.emplace(cpplinq::detail::operators::comparison_result::less_than);
  } else if (left > right) {
    result.emplace(
        cpplinq::detail::operators::comparison_result::greater_than);
  }
  if (left == right) {
    result.emplace(cpplinq::detail::operators::comparison_result::equal);
  } else {
    result.emplace(cpplinq::detail::operators::comparison_result::not_equal);
  }
  return result;
}

template <typename T, typename U>
std::unordered_set<cpplinq::detail::operators::comparison_result> evaluate(
    const T& left,
    const U& right) {
  return {cpplinq::detail::operators::comparison_result::not_equal};
}

}  // namespace cpplinq::detail::operators