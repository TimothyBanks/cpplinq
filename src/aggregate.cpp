#include <cpplinq/detail/aggregates/aggregate.hpp>

using namespace cpplinq::detail::aggregates;

std::string aggregate::value() const {
    return std::visit(overloaded{[](int64_t v) { return std::to_string(v); },
                                    [](double v) { return std::to_string(v); },
                                    [](const std::string& v) { return v; },
                                    [](std::monostate) { return std::string{}; }},
                        accumulator_);
}

std::unordered_map<std::string, aggregate::aggregate_functor> aggregate::aggregrates = {
    {"SUM",
     [](auto& input, auto& accumulator) {
       std::visit(
           overloaded{[&](int64_t v) {
                        accumulator = std::get<int64_t>(accumulator) + v;
                      },
                      [&](double v) {
                        accumulator = std::get<double>(accumulator) + v;
                      },
                      [&](const std::string& v) { /*Does this make sense?*/ },
                      [](std::monostate) {}},
           input);
     }},
    {"COUNT",
     [](auto& input, auto& accumulator) {
       std::visit(
           overloaded{[&](int64_t v) {
                        accumulator = std::get<int64_t>(accumulator) + 1;
                      },
                      [&](double v) {
                        accumulator = std::get<double>(accumulator) + 1;
                      },
                      [&](const std::string& v) { /*Does this make sense?*/ },
                      [](std::monostate) {}},
           input);
     }},
};

bool is_aggregate(std::string aggregate_name) {
  std::transform(std::begin(aggregate_name), std::end(aggregate_name),
                 std::begin(aggregate_name), [](auto c) { return std::toupper(c); });
  return aggregate::aggregrates.count(aggregate_name);
}