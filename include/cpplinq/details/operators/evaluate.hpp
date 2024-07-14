#pragma once
#include <cpplinq/details/operators/expression_tree.hpp>
#include <cpplinq/details/operators/operators.hpp>

namespace cpplinq::details::operators {

template <typename T> bool evaluate(const T &record, const and_operator &op);

template <typename T> bool evaluate(const T &record, const between &op);

template <typename T> bool evaluate(const T &record, const equal_to &op);

template <typename T>
bool evaluate(const T &record, const expression_tree &tree);

template <typename T>
bool evaluate(const T &record, const greater_than_equal &op);

template <typename T> bool evaluate(const T &record, const greater_than &op);

template <typename T> bool evaluate(const T &record, const group &op);

template <typename T> bool evaluate(const T &record, const ilike &op);

template <typename T> bool evaluate(const T &record, const in &op);

template <typename T> bool evaluate(const T &record, const less_than_equal &op);

template <typename T> bool evaluate(const T &record, const less_than &op);

template <typename T> bool evaluate(const T &record, const like &op);

template <typename T> bool evaluate(const T &record, const not_equal &op);

template <typename T> bool evaluate(const T &record, const not_operator &op);

template <typename T> bool evaluate(const T &record, const or_operator &op);

} // namespace cpplinq::details::operators