#include <cpplinq/details/insert_context.hpp>
#include <cpplinq/details/regex.hpp>

namespace cpplinq::details::insert_context {

bool is_this_statement(const std::string& sql)
{
    return cpplinq::regex::begins_with(sql, "INSERT ");
}

cursor execute(const std::string& sql)
{
    return {};
}

}