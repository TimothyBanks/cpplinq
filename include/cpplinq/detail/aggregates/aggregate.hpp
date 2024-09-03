#pragma once
#include <functional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>

namespace cpplinq::detail::aggregates {

template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

struct aggregate {
  using aggregate_type =
      std::variant<std::monostate, int64_t, double, std::string>;
  using aggregate_functor =
      std::function<void(aggregate_type&, aggregate_type&)>;

  static std::unordered_map<std::string, aggregate_functor> aggregrates;

  aggregate_functor functor_;
  aggregate_type accumulator_;

  template <typename T>
  void verify_holds_alternative() {
    if (!std::holds_alternative<T>(accumulator_)) {
      throw "mismatch between accumulator and input types";
    }
  }

  template <typename T>
  void invoke(const T& column_value) {
    auto input = aggregate_type{};

    if constexpr (std::is_integral_v<T>) {
      input = static_cast<int64_t>(column_value);
      verify_holds_alternative<int64_t>();
    } else if constexpr (std::is_floating_point_v<T>) {
      input = static_cast<double>(column_value);
      verify_holds_alternative<double>();
    } else {
      // Assume can be converted to string.
      input = std::to_string(column_value);
      verify_holds_alternative<std::string>();
    }

    functor_(input, accumulator_);
  }

  std::string value() const;
};

bool is_aggregate(std::string aggregate_name);

template <typename T>
inline aggregate make(std::string aggregate_name) {
  std::transform(std::begin(aggregate_name), std::end(aggregate_name),
                 std::begin(aggregate_name), [](auto c) { return std::toupper(c); });

  auto it = aggregate::aggregrates.find(aggregate_name);
  if (it == std::end(aggregate::aggregrates)) {
    return {};
  }

  auto agg = aggregate{};
  agg.functor_ = it->second;

  if constexpr (std::is_integral_v<T>) {
    agg.accumulator_ = static_cast<int64_t>(0);
  } else if constexpr (std::is_floating_point_v<T>) {
    agg.accumulator_ = static_cast<double>(0.0);
  } else {
    // Assume can be converted to string.
    agg.accumulator_ = std::string{};
  }

  return agg;
}
}  // namespace cppling::detail::aggregrates