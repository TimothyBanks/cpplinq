#include <cpplinq/details/call_context.hpp>
#include <cpplinq/details/cpplinq_exception.hpp>
#include <cpplinq/details/regex.hpp>
#include <cpplinq/details/string.hpp>

namespace cpplinq::details {

bool is_call_statement(const std::string& sql) {
  return cpplinq::regex::begins_with(sql, "CALL ");
}

call_context make_call_context(const std::string& sql_) {
  auto context = call_context{};

  auto sql = sql_;
  if (sql.back() == ';') {
    sql.pop_back();
  }

  auto tokens = regex::split(sql, '(');

  if (tokens.size() <= 1) {
    throw cpplinq_exception("Invalid CALL syntax.");
  }

  auto parameters = regex::tokenize(tokens.back(), ',');
  for (const auto& parameter : parameters) {
    if (parameter == ")") {
      break;
    }
    auto subtokens = regex::split(parameter, "=>");
    auto name = std::string{};
    if (subtokens.size() > 1) {
      name = details::string::trim(subtokens.front());
    }
    context.arguments.emplace_back();
    context.arguments.back() =
        std::make_pair(std::move(name), details::string::trim(subtokens.back()));
  }

  tokens = regex::split(tokens.front(), "CALL ");
  context.name = details::string::trim(tokens.back());

  return context;
}

}  // namespace cpplinq::details