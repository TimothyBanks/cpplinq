#pragma once
#include <cpplinq/details/cpplinq_exception.hpp>
#include <cpplinq/details/procedure_registry.hpp>
#include <cpplinq/details/traits/underlying_column_type.hpp>
#include <tuple>
#include <vector>

// __param_tuple__  = (name, type, [default])
#define DECLARE_PARAMETER(__ignored__, __procedure_name__, __I__,             \
                          __param_tuple__)                                    \
  template <>                                                                 \
  struct parameter_trait<cpplinq::details::hash(                              \
      BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(BOOST_PP_TUPLE_SIZE(__param_tuple__), 0, __param_tuple__))),        \
      cpplinq::details::hash(__procedure_name__)> {                           \
    constexpr static auto hash =                                              \
        cpplinq::details::hash(BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(BOOST_PP_TUPLE_SIZE(__param_tuple__), 0, __param_tuple__)));      \
    constexpr static auto procedure_hash =                                    \
        cpplinq::details::hash(__procedure_name__);                           \
    constexpr static auto position = size_t{__I__};                           \
    using type = BOOST_PP_TUPLE_ELEM(BOOST_PP_TUPLE_SIZE(__param_tuple__),    \
                                     1,                                       \
                                     __param_tuple__);                        \
    static const auto default_value = type{                                   \
        BOOST_PP_IF(BOOST_PP_TUPLE_SIZE(__param_tuple__) == 3,                \
                    BOOST_PP_TUPLE_ELEM(BOOST_PP_TUPLE_SIZE(__param_tuple__), \
                                        2,                                    \
                                        __param_tuple__),                     \
                    _)};                                                      \
                                                                              \
    static const type from_string(const std::: string& value) {               \
      if (value.empty()) {                                                    \
        return default_value;                                                 \
      }                                                                       \
      return cpplinq::details::traits::underlying_column_type<                \
          type>::from_string(value);                                          \
    }                                                                         \
    static const std::string& procedure_name() {                              \
      static const auto name_ = std::string{__procedure_name__};              \
      return name_;                                                           \
    }                                                                         \
    static const std::string& name() {                                        \
      static const auto name_ =                                               \
          std::string{BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(BOOST_PP_TUPLE_SIZE(__param_tuple__), 0, __param_tuple__))};    \
      return name_;                                                           \
    }                                                                         \
  };

#define CONVERT_PARAMETER(__ignored_1__, __ignored_2__, __I__,    \
                          __param_tuple__)                        \
  parameter_trait<cpplinq::details::hash(BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(BOOST_PP_TUPLE_SIZE(__param_tuple__), 0, __param_tuple__))), \
      hash>::from_string(all_args[__I__]),

#define GET_PARAMETER_NAME(__ignored_1__, __ignored_2__, __parameter_tuple__) \
  BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(BOOST_PP_TUPLE_SIZE(__parameter_tuple__), 0, __parameter_tuple__)),

// __procedure_tuple__
//     = (procedure_type, procedure_name, ((param1, type1,
//     [default]))...((paramN, typeN, [default])))
#define DECLARE_PROCEDURE(__procedure_type__, __procedure_name__,             \
                          __parameters__)                                     \
  BOOST_PP_SEQ_FOR_EACH_I(                                                    \
      DECLARE_PARAMETER,                                                      \
      BOOST_PP_CAT(BOOST_PP_STRINGIZE(__procedure_type__),                                                \
          BOOST_PP_CAT(., BOOST_PP_STRINGIZE(__procedure_name__))),           \
                   __parameters__)                                            \
  template <>                                                                 \
  struct procedure_trait<                                                     \
      __procedure_type__,                                                     \
      cpplinq::details::hash(BOOST_PP_CAT(BOOST_PP_STRINGIZE(__procedure_type__),                                                \
          BOOST_PP_CAT(., BOOST_PP_STRINGIZE(__procedure_name__))))> {        \
    using procedure_type = __procedure_type__;                                \
    static constexpr auto hash =                                              \
        cpplinq::details::hash(BOOST_PP_CAT(BOOST_PP_STRINGIZE(__procedure_type__),                                              \
            BOOST_PP_CAT(., BOOST_PP_STRINGIZE(__procedure_name__))));        \
                                                                              \
    static const std::vector<std::string>& parameters() {                     \
      static const auto parameters_ = std::vector<std::string>{               \
          BOOST_PP_SEQ_FOR_EACH(GET_PARAMETER_NAME, _, __parameters__)};      \
      return parameters_;                                                     \
    }                                                                         \
    static const std::string& name() {                                        \
      static const auto name_ = std::string{BOOST_PP_CAT(BOOST_PP_STRINGIZE(__procedure_type__),                                                \
          BOOST_PP_CAT(., BOOST_PP_STRINGIZE(__procedure_name__)))};          \
      return name_;                                                           \
    }                                                                         \
    static cpplinq::details::cursor invoke(const procedure_arguments& args) { \
      auto all_args = reconcile<procedure_trait<procedure_type, hash>>(args); \
      return __procedure_type__::__procedure_name__(                          \
          BOOST_PP_SEQ_FOR_EACH_I(CONVERT_PARAMETER, _, __parameters__));     \
    }                                                                         \
  };                                                                          \
  static auto BOOST_PP_CAT(                                                   \
      registered_,                                                            \
      BOOST_PP_CAT(BOOST_PP_STRINGIZE(__procedure_type__),                                   \
                       BOOST_PP_CAT(_, BOOST_PP_STRINGIZE(__procedure_name__)))) = []() {    \
        using trait = procedure_trait<                                        \
            __procedure_type__,                                               \
            cpplinq::details::hash(BOOST_PP_CAT(BOOST_PP_STRINGIZE(__procedure_type__),                                          \
                BOOST_PP_CAT(_, BOOST_PP_STRINGIZE(__procedure_name__))))>;   \
        cpplinq::details::procedure_registry::instance().add(                 \
            trait::name(), cpplinq::details::traits::any_procedure{trait{}}); \
        return true;                                                          \
      }();

namespace cpplinq::details::traits {

template <size_t Hash, size_t Procedure_hash>
struct parameter_trait;

template <typename Procedure_type, size_t Hash>
struct procedure_trait;

using procedure_arguments = std::vector<std::pair<std::string, std::string>>;

// This can be done in the any_procedure type
template <typename Procedure_trait>
procedure_arguments reconcile(const procedure_arguments& args) {
  auto all_arguments = procedure_arguments{};
  for (const auto& arg : Procedure_trait::arguments()) {
    all_arguments.emplace_back();
    auto& back = all_arguments.back();
    back.first = arg;
  }

  auto positional = true;
  for (auto i = size_t{0}; i < args.size(); ++i) {
    auto& t = args[i];

    if (t.first.empty()) {
      if (!positional) {
        throw cpplinq_exception(
            "Positional argument after named "
            "argument");
      }
      all_arguments[i].second = t.second;
      continue;
    }

    positional = false;
    for (auto& arg : all_arguments) {
      if (arg.first != t.first) {
        continue;
      }
      arg.second = t.second;
      break;
    }
  }

  return all_arguments;
}

}  // namespace cpplinq::details::traits