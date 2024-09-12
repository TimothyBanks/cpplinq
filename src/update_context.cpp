#include <cpplinq/detail/operators/make.hpp>
#include <cpplinq/detail/regex.hpp>
#include <cpplinq/detail/string.hpp>
#include <cpplinq/detail/update_context.hpp>

namespace cpplinq::detail {

bool is_update_statement(const std::string& sql) {
  return cpplinq::regex::begins_with(sql, "UPDATE ");
}

update_context make_update_context(const std::string& sql_) {
  // clang-format off
  /***************************
  General format of a UPDATE statement in PostgreSQL:
  UPDATE table_name 
  SET [column1] = [value1], ..., [columnN] = [valueN] 
  WHERE condition
  RANGE index_name LOWER_BOUND [column1 value, column2 value, ...] UPPER_BOUND [column1 value, column2 value, ...];
  
  NOTE:  Currently only literal values are supported within the SET clause.
  ****************************/
  // clang-format on

  auto context = update_context{};

  auto sql = sql_;
  if (sql.back() == ';') {
    sql.pop_back();
  }

  // While it would be much better to use a lexer and parser such as boost
  // spirit, for now we tokenize the string by starting from the OFFSET keyword
  // and popping off the last part of the statement.

  auto tokens = regex::split(sql, " RANGE ");
  if (tokens.size() > 1) {
    context.range = make_range_info(tokens.back());
  }

  tokens = regex::split(tokens.front(), " WHERE ");
  if (tokens.size() > 1) {
    // Build the expression tree from the conditionals in the WHERE
    context.et = detail::operators::make_expression_tree(
        cpplinq::detail::string::trim(tokens.back()));
  }

  tokens = regex::split(tokens.front(), " SET ");
  if (tokens.size() > 1) {
    auto subtokens = regex::split(tokens.back(), ',');
    for (auto& token : subtokens) {
      auto parts = regex::split(token, '=');
      if (parts.size() != 2) {
        // Ignore for now.
        continue;
      }
      context.values.push_back(
          {.name = cpplinq::detail::string::trim(parts.front()),
           .value = cpplinq::detail::string::trim(parts.back())});
    }
  }

  tokens = regex::split(tokens.front(), "UPDATE ");
  if (tokens.size() > 1) {
    auto subtokens = regex::split(tokens.back(), " AS ");
    context.table_name = cpplinq::detail::string::trim(subtokens.front());
    context.table_alias = subtokens.size() > 1
                              ? cpplinq::detail::string::trim(subtokens.back())
                              : std::string{};
  }

  return context;
}

}  // namespace cpplinq::detail