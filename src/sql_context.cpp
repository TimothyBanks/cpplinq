#include <cpplinq/details/call_context.hpp>
#include <cpplinq/details/delete_context.hpp>
#include <cpplinq/details/insert_context.hpp>
#include <cpplinq/details/select_context.hpp>
#include <cpplinq/details/update_context.hpp>
#include <cpplinq/sql_context.hpp>

namespace cpplinq::sql_context {

cpplinq::details::cursor execute(const std::string& sql) {
    if (details::call_context::is_this_statement(sql)) {
        return details::call_context::execute(sql);
    }
    if (details::delete_context::is_this_statement(sql)) {
        return details::delete_context::execute(sql);
    }
    if (details::insert_context::is_this_statement(sql)) {
        return details::insert_context::execute(sql);
    }
    if (details::select_context::is_this_statement(sql)) {
        return details::select_context::execute(sql);
    }
    if (details::update_context::is_this_statement(sql)) {
        return details::update_context::execute(sql);
    }
    return {};
}

}