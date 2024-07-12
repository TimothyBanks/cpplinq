#include <cpplinq/details/update_context.hpp>
#include <cpplinq/details/regex.hpp>

namespace cpplinq::details::update_context {

bool is_this_statement(const std::string& sql)
{
    return cpplinq::regex::begins_with(sql, "UPDATE ");
}

cursor execute(const std::string& sql)
{
    return {};
}

}