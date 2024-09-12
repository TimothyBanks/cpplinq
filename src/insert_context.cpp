#include <cpplinq/detail/insert_context.hpp>
#include <cpplinq/detail/regex.hpp>
#include <cpplinq/detail/string.hpp>

namespace cpplinq::detail {

bool is_insert_statement(const std::string& sql) {
  return cpplinq::regex::begins_with(sql, "INSERT INTO ");
}

insert_context make_insert_context(const std::string& sql_) {
  // clang-format off
  /***************************
  General format of a UPDATE statement in PostgreSQL:
  INSERT INTO table_name (column1, ..., columnN)
  VALUES (value1_1, ..., valueN_1), ..., (value1_M, ..., valueN_M);
  
  NOTE:  Currently only literal values are supported within the VALUES clause.
  ****************************/
  // clang-format on

  auto context = insert_context{};

  auto sql = sql_;
  if (sql.back() == ';') {
    sql.pop_back();
  }

  // While it would be much better to use a lexer and parser such as boost
  // spirit, for now we tokenize the string by starting from the OFFSET keyword
  // and popping off the last part of the statement.

  auto tokens = regex::split(sql, " VALUES ");
  if (tokens.size() > 1) {
    auto values = regex::tokenize(tokens.back(), "),");
    if (!values.empty()) {
      for (auto& value : values) {
        auto columns = regex::tokenize(value, ',');
        context.values.emplace_back();
        auto& row = context.values.back();
        for (auto& column : columns) {
          row.push_back(cpplinq::detail::string::trim(column));
        }
      }
    }
  }

  tokens = regex::split(tokens.front(), " (");
  if (tokens.size() > 1) {
    auto columns = regex::tokenize(tokens.back(), ',');
    for (auto& column : columns) {
      context.columns.push_back(
          {.name = cpplinq::detail::string::trim(column)});
    }
  }

  tokens = regex::split(tokens.front(), "INSERT INTO ");
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