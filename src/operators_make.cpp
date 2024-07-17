#include <cpplinq/details/cpplinq_exception.hpp>
#include <cpplinq/details/operators/make.hpp>
#include <cpplinq/details/operators/operators.hpp>
#include <cpplinq/details/regex.hpp>
#include <cpplinq/details/string.hpp>

namespace cpplinq::details::operators {

operator_ptr make_operator(std::string& sql);

operator_ptr make_and_operator(std::string& left, std::string& right);

operator_ptr make_between(std::string& left, std::string& right);

operator_ptr make_equal(std::string& left, std::string& right);

operator_ptr make_greater_than_equal(std::string& left, std::string& right);

operator_ptr make_greater_than(std::string& left, std::string& right);

operator_ptr make_group(std::string& sql);

operator_ptr make_ilike(std::string& left, std::string& right);

operator_ptr make_in(std::string& left, std::string& right);

operator_ptr make_is(std::string& left, std::string& right);

operator_ptr make_less_than_equal(std::string& left, std::string& right);

operator_ptr make_less_than(std::string& left, std::string& right);

operator_ptr make_like(std::string& left, std::string& right);

operator_ptr make_not_equal(std::string& left, std::string& right);

operator_ptr make_not_operator(std::string& left, std::string& right);

operator_ptr make_or_operator(std::string& left, std::string& right);

void push_groups(std::string& sql, std::vector<std::string>& groups) {
  auto new_sql = std::string{};
  for (auto i = size_t{0}; i < sql.size(); ++i) {
    if (sql[i] == '{') {
      auto start = i;
      while (sql[++i] != '}') {
      };
      auto index = std::atoi(sql.c_str() + start + 1);
      new_sql += groups[index];
    } else {
      new_sql += sql[i];
    }
  }
  sql = new_sql;
}

std::vector<std::string> pop_groups(std::string& sql) {
  static constexpr auto not_set = static_cast<size_t>(-1);
  auto position = not_set;
  auto count = size_t{0};
  auto result = std::vector<std::string>{};

  auto new_sql = std::string{};
  new_sql.reserve(sql.size());

  for (auto i = size_t{0}; i < sql.size(); ++i) {
    if (sql[i] == '(') {
      if (position == not_set) {
        position = i;
      }
      ++count;
    } else if (sql[i] == ')' && --count == 0) {
      auto length = i - position + 1;
      auto grouping = sql.substr(position, length);
      new_sql += "{" + std::to_string(result.size()) + "}";
      result.emplace_back(std::move(grouping));
      position = not_set;
    } else if (position == not_set) {
      new_sql += sql[i];
    }
  }

  sql = new_sql;
  return result;
}

operator_ptr make_operator(std::string& sql) {
  // First step is we need to handle parenthesis.  The idea will be
  // to "erase" things between parenthesis to correctly parse things
  // in a top down fashion.
  auto groups = pop_groups(sql);

  // Follow an order of precendence here:
  // 0. Parenthesis
  // 1. AND
  // 2. OR
  // 3. NOT
  // 4. Any of the comparison operators (<, <=, >, >=, <>, !=, IS NULL, IS NOT
  // NULL, ILIKE, LIKE, BETWEEN, IN)
  if (sql == "{0}") {
    // The whole thing is a grouping
    push_groups(sql, groups);
    auto op = make_group(sql);
    op->original_statement = sql;
    return op;
  }

  auto make = [&](auto token, auto functor) {
    auto tokens = regex::split(sql, token);
    if (tokens.size() > 1) {
      push_groups(tokens.front(), groups);
      push_groups(tokens.back(), groups);
      auto original_statement = tokens.front() + token + tokens.back();
      auto result = functor(tokens.front(), tokens.back());
      result->original_statement = std::move(original_statement);
      return result;
    }
    return operator_ptr{};
  };

  if (auto ptr = make(" AND ", [](auto& left, auto& right) {
        return make_and_operator(left, right);
      })) {
    return ptr;
  }

  if (auto ptr = make(" OR ", [](auto& left, auto& right) {
        return make_or_operator(left, right);
      })) {
    return ptr;
  }

  //   if (auto ptr = make(" NOT ", [](auto &left, auto &right) {
  //         return make_not_operator(left, right);
  //       })) {
  //     return ptr;
  //   }

  if (auto ptr = make(" < ", [](auto& left, auto& right) {
        return make_less_than(left, right);
      })) {
    return ptr;
  }

  if (auto ptr = make(" <= ", [](auto& left, auto& right) {
        return make_less_than_equal(left, right);
      })) {
    return ptr;
  }

  if (auto ptr = make(" > ", [](auto& left, auto& right) {
        return make_greater_than(left, right);
      })) {
    return ptr;
  }

  if (auto ptr = make(" >= ", [](auto& left, auto& right) {
        return make_greater_than_equal(left, right);
      })) {
    return ptr;
  }

  if (auto ptr = make(" = ", [](auto& left, auto& right) {
        return make_equal(left, right);
      })) {
    return ptr;
  }

  if (auto ptr = make(" != ", [](auto& left, auto& right) {
        return make_not_equal(left, right);
      })) {
    return ptr;
  }

  //   if (auto ptr = make(" IS ", [](auto &left, auto &right) {
  //         return make_is(left, right);
  //       })) {
  //     return ptr;
  //   }

  if (auto ptr = make(" ILIKE ", [](auto& left, auto& right) {
        return make_ilike(left, right);
      })) {
    return ptr;
  }

  if (auto ptr = make(" LIKE ", [](auto& left, auto& right) {
        return make_like(left, right);
      })) {
    return ptr;
  }

  if (auto ptr = make(" BETWEEN ", [](auto& left, auto& right) {
        return make_between(left, right);
      })) {
    return ptr;
  }

  if (auto ptr = make(" IN ", [](auto& left, auto& right) {
        return make_in(left, right);
      })) {
    return ptr;
  }

  push_groups(sql, groups);
  throw cpplinq::details::cpplinq_exception{"Unsupported SQL syntax"};
}

operator_ptr make_and_operator(std::string& left, std::string& right) {
  auto op = std::make_unique<and_operator>();
  op->type = cpplinq::details::operators::operator_type::and_;
  op->left_operand = make_operator(left);
  op->right_operand = make_operator(right);
  return op;
}

operator_ptr make_between(std::string& left, std::string& right) {
  auto op = std::make_unique<between>();
  op->type = cpplinq::details::operators::operator_type::between;
  op->column = std::move(cpplinq::details::string::trim(left));
  right = cpplinq::details::string::trim(right);
  auto tokens = regex::split(right, " AND ");
  if (tokens.size() != 2) {
    throw cpplinq::details::cpplinq_exception{"Unsupported SQL syntax"};
  }
  op->begin = tokens.front();
  op->end = tokens.back();
  return op;
}

operator_ptr make_equal(std::string& left, std::string& right) {
  auto op = std::make_unique<equal_to>();
  op->type = cpplinq::details::operators::operator_type::equal_to;
  op->column_name = std::move(cpplinq::details::string::trim(left));
  op->value = std::move(cpplinq::details::string::trim(right)); 
  return op;
}

expression_tree make_expression_tree(std::string sql) {
  auto t = expression_tree{};
  t.root = make_operator(sql);
  return t;
}

operator_ptr make_greater_than_equal(std::string& left, std::string& right) {
  auto op = std::make_unique<greater_than_equal>();
  op->type = cpplinq::details::operators::operator_type::greater_than_equal;
  op->column_name = std::move(cpplinq::details::string::trim(left));
  op->value = std::move(cpplinq::details::string::trim(right));
  return op;
}

operator_ptr make_greater_than(std::string& left, std::string& right) {
  auto op = std::make_unique<greater_than>();
  op->type = cpplinq::details::operators::operator_type::greater_than;
  op->column_name = std::move(cpplinq::details::string::trim(left));
  op->value = std::move(cpplinq::details::string::trim(right));
  return op;
}

operator_ptr make_group(std::string& sql) {
  auto group_sql = sql.substr(1);
  group_sql.pop_back();
  auto op = std::make_unique<group>();
  op->type = cpplinq::details::operators::operator_type::group;
  op->root = make_operator(group_sql);
  return op;
}

operator_ptr make_ilike(std::string& left, std::string& right) {
  auto op = std::make_unique<ilike>();
  op->type = cpplinq::details::operators::operator_type::ilike;
  op->column_name = std::move(cpplinq::details::string::trim(left));
  op->value = std::move(cpplinq::details::string::trim(right));
  return op;
}

operator_ptr make_in(std::string& left, std::string& right) {
  auto op = std::make_unique<in>();
  op->type = cpplinq::details::operators::operator_type::in;
  op->column = std::move(cpplinq::details::string::trim(left));
  right = std::move(cpplinq::details::string::trim(right));
  right = right.substr(1);
  right.pop_back();
  auto values = regex::tokenize(right, ',');
  for (auto& value : values) {
    op->values.emplace(std::move(cpplinq::details::string::trim(value)));
  }
  return {};
}

operator_ptr make_is(std::string& left, std::string& right) {
  throw cpplinq::details::cpplinq_exception{"Unsupported SQL syntax"};
}

operator_ptr make_less_than_equal(std::string& left, std::string& right) {
  auto op = std::make_unique<less_than_equal>();
  op->type = cpplinq::details::operators::operator_type::less_than_equal;
  op->column_name = std::move(cpplinq::details::string::trim(left));
  op->value = std::move(cpplinq::details::string::trim(right));
  return op;
}

operator_ptr make_less_than(std::string& left, std::string& right) {
  auto op = std::make_unique<less_than>();
  op->type = cpplinq::details::operators::operator_type::less_than;
  op->column_name = std::move(cpplinq::details::string::trim(left));
  op->value = std::move(cpplinq::details::string::trim(right));
  return op;
}

operator_ptr make_like(std::string& left, std::string& right) {
  auto op = std::make_unique<like>();
  op->type = cpplinq::details::operators::operator_type::like;
  op->column_name = std::move(cpplinq::details::string::trim(left));
  op->value = std::move(cpplinq::details::string::trim(right));
  return op;
}

operator_ptr make_not_equal(std::string& left, std::string& right) {
  auto op = std::make_unique<not_equal>();
  op->type = cpplinq::details::operators::operator_type::not_equal;
  op->column_name = std::move(cpplinq::details::string::trim(left));
  op->value = std::move(cpplinq::details::string::trim(right));
  return op;
}

operator_ptr make_not_operator(std::string& left, std::string& right) {
  throw cpplinq::details::cpplinq_exception{"Unsupported SQL syntax"};
}

operator_ptr make_or_operator(std::string& left, std::string& right) {
  auto op = std::make_unique<or_operator>();
  op->type = cpplinq::details::operators::operator_type::or_;
  op->left_operand = make_operator(cpplinq::details::string::trim(left));
  op->right_operand = make_operator(cpplinq::details::string::trim(right));
  return op;
}

}  // namespace cpplinq::details::operators