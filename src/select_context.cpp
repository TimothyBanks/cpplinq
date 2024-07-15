#include <cpplinq/details/column.hpp>
#include <cpplinq/details/operators/make.hpp>
#include <cpplinq/details/regex.hpp>
#include <cpplinq/details/select_context.hpp>
#include <cpplinq/details/string.hpp>
#include <cpplinq/details/table.hpp>
#include <cpplinq/details/unsupported.hpp>

namespace cpplinq::details {

bool is_select_statement(const std::string& sql) {
  return cpplinq::regex::begins_with(sql, "SELECT ");
}

select_context make_select_context(const std::string& sql) {
  /***************************
  General format of a SELECT statement in PostgreSQL:
  SELECT DISTINCT column1, column2, ...
  FROM table_name
  JOIN other_table ON table_name.column = other_table.column
  WHERE condition
  GROUP BY column1, column2, ...
  HAVING condition
  ORDER BY column1 [ASC|DESC], column2 [ASC|DESC], ...
  LIMIT number OFFSET number;
  ****************************/

  auto context = select_context{};

  // While it would be much better to use a lexer and parser such as boost
  // spirit, for now we tokenize the string by starting from the OFFSET keyword
  // and popping off the last part of the statement.
  auto tokens = regex::split(sql, " OFFSET ");
  if (tokens.size() > 1) {
    context.offset = static_cast<size_t>(std::atoll(tokens.back().c_str()));
  }

  tokens = regex::split(tokens.front(), " LIMIT ");
  if (tokens.size() > 1) {
    context.limit = static_cast<size_t>(std::atoll(tokens.back().c_str()));
  }

  check_unsupported_token(tokens.front(), " ORDER BY ");
  check_unsupported_token(tokens.front(), " HAVING ");
  check_unsupported_token(tokens.front(), " GROUP BY ");

  tokens = regex::split(tokens.front(), " WHERE ");
  if (tokens.size() > 1) {
    // Build the expression tree from the conditionals in the WHERE
    context.et = details::operators::make_expression_tree(tokens.back());
  }

  check_unsupported_token(tokens.front(), " JOIN ");

  tokens = regex::split(tokens.front(), " FROM ");
  if (tokens.size() > 1) {
    auto subtokens = regex::split(tokens.back(), " AS ");
    context.table_name = cpplinq::details::string::trim(subtokens.front());
    context.table_alias = subtokens.size() > 1
                              ? cpplinq::details::string::trim(subtokens.back())
                              : std::string{};
  }

  check_unsupported_token(tokens.front(), " DISTINCT ");

  tokens = regex::split(tokens.front(), "SELECT ");
  // TODO:  Add a check for "*" and just populate the columns with all available
  // table columns.
  auto column_tokens = regex::split(tokens.back(), ',');
  for (auto& c : column_tokens) {
    auto subtokens = regex::split(c, " AS ");

    context.columns.emplace_back();
    auto& new_column = context.columns.back();

    // TODO:  If JOIN becomes supported, will need to parse table.column_name in
    // SELECT.
    new_column.table =
        context.table_alias.empty() ? context.table_name : context.table_alias;
    new_column.alias = subtokens.size() > 1
                           ? cpplinq::details::string::trim(subtokens.back())
                           : std::string{};

    auto column_token = subtokens.front();
    subtokens = regex::split(column_token, '(');
    if (subtokens.size() > 1) {
      // This is an aggregate function.
      new_column.aggregate = subtokens.front();
      subtokens = regex::split(subtokens.back(), ')');
      new_column.name =
          subtokens.front().empty() ? new_column.alias : subtokens.front();
    } else {
      new_column.name = cpplinq::details::string::trim(column_token);
    }
  }

  return context;
}

}  // namespace cpplinq::details