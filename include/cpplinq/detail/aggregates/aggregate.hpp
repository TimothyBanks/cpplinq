#pragma once
#include <any>
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
      std::function<void(aggregate_type&, aggregate&)>;

  static std::unordered_map<std::string, aggregate_functor> aggregrates;

  aggregate_functor functor_;
  aggregate_type accumulator_;
  std::string name_;

  operator bool() {
    return !std::holds_alternative<std::monostate>(accumulator_);
  }

  void verify_holds_alternative(const aggregate_type& input);
  void initialize(const aggregate_type& input);

  template <typename T>
  void accumulate(const T& column_value) {
    auto input = aggregate_type{};

    if constexpr (std::is_integral_v<T>) {
      input = static_cast<int64_t>(column_value);
    } else if constexpr (std::is_floating_point_v<T>) {
      input = static_cast<double>(column_value);
    } else if constexpr (std::is_same_v<std::decay_t<T>, std::string>) {
      input = column_value;
    } else {
      // Assume can be converted to string.
      // input = std::to_string(column_value);

      // For now, just bail out.  We can also extend this to have a library of to_string overloads
      // to force support
      return;
    }

    functor_(input, *this);
  }

  std::any value() const;
};

bool is_aggregate(std::string aggregate_name);
aggregate make(std::string aggregate_name);

}  // namespace cppling::detail::aggregrates