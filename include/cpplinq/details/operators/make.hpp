#pragma once
#include <cpplinq/details/operators/expression_tree.hpp>
#include <cpplinq/details/operators/operator_base.hpp>

namespace cpplinq::details::operators {

expression_tree make_expression_tree(std::string sql);

}