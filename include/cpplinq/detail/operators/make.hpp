#pragma once
#include <cpplinq/detail/operators/expression_tree.hpp>
#include <cpplinq/detail/operators/operator_base.hpp>

namespace cpplinq::detail::operators {

expression_tree make_expression_tree(std::string sql);

}