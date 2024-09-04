#include <cpplinq/detail/aggregates/aggregate.hpp>

namespace cpplinq::detail::aggregates {

std::any aggregate::value() const {
  return std::visit(
      overloaded{[](std::monostate) { return std::any{std::string{}}; },
                 [](const auto& v) { return std::any{v}; }},
      accumulator_);
}

void aggregate::initialize(const aggregate_type& input) {
  if (std::holds_alternative<std::monostate>(accumulator_)) {
    std::visit(
        overloaded{[&](int64_t v) { accumulator_ = static_cast<int64_t>(0); },
                   [&](double v) { accumulator_ = static_cast<double>(0); },
                   [&](const auto& v) { accumulator_ = std::string{}; }},
        input);
  }
}

void aggregate::verify_holds_alternative(const aggregate_type& input) {
  auto result = false;
  std::visit(
      overloaded{[&](int64_t v) {
                   result = std::holds_alternative<int64_t>(accumulator_);
                 },
                 [&](double v) {
                   result = std::holds_alternative<double>(accumulator_);
                 },
                 [&](const std::string& v) {
                   result = std::holds_alternative<std::string>(accumulator_);
                 },
                 [&](std::monostate) {
                   result =
                       std::holds_alternative<std::monostate>(accumulator_);
                 }},
      input);

  if (!result) {
    throw "mismatch between accumulator and input types";
  }
}

std::unordered_map<std::string, aggregate::aggregate_functor>
    aggregate::aggregrates = {
        {"SUM",
         [](aggregate_type& input, aggregate& accumulator) {
           accumulator.initialize(input);
           accumulator.verify_holds_alternative(input);

           std::visit(
               overloaded{
                   [&](int64_t v) {
                     accumulator.accumulator_ =
                         std::get<int64_t>(accumulator.accumulator_) + v;
                   },
                   [&](double v) {
                     accumulator.accumulator_ =
                         std::get<double>(accumulator.accumulator_) + v;
                   },
                   [&](const std::string& v) { /*Does this make sense?*/ },
                   [](std::monostate) {}},
               input);
         }},
        {"COUNT",
         [](aggregate_type& input, aggregate& accumulator) {
           if (std::holds_alternative<std::monostate>(
                   accumulator.accumulator_)) {
             // Initialize the accumulator.
             accumulator.accumulator_ = static_cast<int64_t>(0);
           }
           accumulator.accumulator_ =
               std::get<int64_t>(accumulator.accumulator_) + 1;
         }},
};

bool is_aggregate(std::string aggregate_name) {
  std::transform(std::begin(aggregate_name), std::end(aggregate_name),
                 std::begin(aggregate_name),
                 [](auto c) { return std::toupper(c); });
  return aggregate::aggregrates.count(aggregate_name);
}

aggregate make(std::string aggregate_name) {
  std::transform(std::begin(aggregate_name), std::end(aggregate_name),
                 std::begin(aggregate_name),
                 [](auto c) { return std::toupper(c); });

  auto it = aggregate::aggregrates.find(aggregate_name);
  if (it == std::end(aggregate::aggregrates)) {
    return {};
  }

  auto agg = aggregate{};
  agg.name_ = it->first;
  agg.functor_ = it->second;
  return agg;
}
}  // namespace cpplinq::detail::aggregates