#include <cpplinq/detail/column.hpp>
#include <cpplinq/detail/operators/make.hpp>
#include <cpplinq/detail/regex.hpp>
#include <cpplinq/detail/select_context.hpp>
#include <cpplinq/detail/string.hpp>
#include <cpplinq/detail/table.hpp>
#include <cpplinq/detail/table_registry.hpp>
#include <cpplinq/detail/unsupported.hpp>

namespace cpplinq::detail {

bool is_select_statement(const std::string& sql) {
  return cpplinq::regex::begins_with(sql, "SELECT ");
}

select_context make_select_context(const std::string& sql_) {
  /***************************
  General format of a SELECT statement in PostgreSQL:
  SELECT DISTINCT column1, column2, ...
  FROM table_name
  JOIN other_table ON table_name.column = other_table.column
  WHERE condition
  RANGE index_name LOWER_BOUND [column1 value, column2 value, ...] UPPER_BOUND
  [column1 value, column2 value, ...]) GROUP BY column1, column2, ... HAVING
  condition ORDER BY column1 [ASC|DESC], column2 [ASC|DESC], ... LIMIT number
  OFFSET number;
  ****************************/

  auto context = select_context{};

  auto sql = sql_;
  if (sql.back() == ';') {
    sql.pop_back();
  }

  // While it would be much better to use a lexer and parser such as boost
  // spirit, for now we tokenize the string by starting from the OFFSET keyword
  // and popping off the last part of the statement.
  auto tokens = regex::split(sql, " OFFSET ");
  if (tokens.size() > 1) {
    context.offset = static_cast<size_t>(
        std::atoll(cpplinq::detail::string::trim(tokens.back()).c_str()));
  }

  tokens = regex::split(tokens.front(), " LIMIT ");
  if (tokens.size() > 1) {
    context.limit = static_cast<size_t>(
        std::atoll(cpplinq::detail::string::trim(tokens.back()).c_str()));
  }

  check_unsupported_token(tokens.front(), " ORDER BY ");
  check_unsupported_token(tokens.front(), " HAVING ");
  check_unsupported_token(tokens.front(), " GROUP BY ");

  tokens = regex::split(tokens.front(), " RANGE ");
  if (tokens.size() > 1) {
    auto range = select_context::range_info{};

    auto extract = [](auto& s) {
      auto trimmed = cpplinq::detail::string::trim(s);
      trimmed.pop_back();
      trimmed = trimmed.substr(1);
      auto tokens = regex::tokenize(trimmed, ',');
      for (auto& c : tokens) {
        c = cpplinq::detail::string::trim(c);
      }
      return tokens;
    };

    auto subtokens = regex::split(tokens.back(), " UPPER_BOUND ");
    if (subtokens.size() > 1) {
      // There is an upper bound clause.
      range.upper_bound = extract(subtokens.back());
    }

    subtokens = regex::split(subtokens.front(), " LOWER_BOUND ");
    if (subtokens.size() > 1) {
      // There is a lower bound clause.
      range.lower_bound = extract(subtokens.back());
    }

    range.index_name = cpplinq::detail::string::trim(subtokens.front());
    context.range = std::move(range);
  }

  tokens = regex::split(tokens.front(), " WHERE ");
  if (tokens.size() > 1) {
    // Build the expression tree from the conditionals in the WHERE
    context.et = detail::operators::make_expression_tree(
        cpplinq::detail::string::trim(tokens.back()));
  }

  check_unsupported_token(tokens.front(), " JOIN ");

  tokens = regex::split(tokens.front(), " FROM ");
  if (tokens.size() > 1) {
    auto subtokens = regex::split(tokens.back(), " AS ");
    context.table_name = cpplinq::detail::string::trim(subtokens.front());
    context.table_alias = subtokens.size() > 1
                              ? cpplinq::detail::string::trim(subtokens.back())
                              : std::string{};
  }

  auto& table =
      cpplinq::detail::table_registry::instance().find(context.table_name);

  check_unsupported_token(tokens.front(), " DISTINCT ");

  tokens = regex::split(tokens.front(), "SELECT ");
  auto column_tokens = regex::tokenize(tokens.back(), ',');
  if (column_tokens.size() == 1 &&
      cpplinq::detail::string::trim(column_tokens.front()) == "*") {
    for (const auto& column_name : table.columns()) {
      context.columns.emplace_back();
      auto& new_column = context.columns.back();
      new_column.name = column_name;
      new_column.table = context.table_alias.empty() ? context.table_name
                                                     : context.table_alias;
    }
  } else {
    for (auto& c : column_tokens) {
      auto subtokens = regex::split(c, " AS ");

      context.columns.emplace_back();
      auto& new_column = context.columns.back();

      // TODO:  If JOIN becomes supported, will need to parse table.column_name
      // in SELECT.
      new_column.table = context.table_alias.empty() ? context.table_name
                                                     : context.table_alias;
      new_column.alias = subtokens.size() > 1
                             ? cpplinq::detail::string::trim(subtokens.back())
                             : std::string{};

      auto column_token = subtokens.front();
      subtokens = regex::split(column_token, '(');
      if (subtokens.size() > 1) {
        // This is an aggregate function.
        new_column.aggregate = cpplinq::detail::string::trim(subtokens.front());
        subtokens = regex::split(subtokens.back(), ')');
        new_column.name =
            subtokens.front().empty()
                ? new_column.alias
                : cpplinq::detail::string::trim(subtokens.front());
      } else {
        new_column.name = cpplinq::detail::string::trim(column_token);
      }
    }
  }

  return context;
}

}  // namespace cpplinq::detail