#pragma once
#include <cpplinq/details/cursor.hpp>
#include <string_view>
#include <tuple>

namespace cpplinq::details {

struct call_context {
    std::string name;
    std::vector<std::pair<std::string, std::string>>arguments;
};

bool is_call_statement(const std::string& sql);
call_context make_call_context(const std::string& sql);

}  // namespace cpplinq::details