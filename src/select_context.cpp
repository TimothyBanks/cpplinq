#include <cpplinq/details/select_context.hpp>
#include <cpplinq/details/regex.hpp>

namespace cpplinq::details::select_context {

bool is_this_statement(const std::string& sql)
{
    return cpplinq::regex::begins_with(sql, "SELECT ");
}

cursor execute(const std::string& sql)
{
    return {};
}

}