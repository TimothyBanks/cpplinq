#include <cpplinq/details/call_context.hpp>
#include <cpplinq/details/regex.hpp>

namespace cpplinq::details::call_context {

bool is_this_statement(const std::string& sql)
{
    return cpplinq::regex::begins_with(sql, "CALL ");
}

cursor execute(const std::string& sql)
{
    return {};
}

}