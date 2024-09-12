#pragma once
#include <cpplinq/detail/cpplinq_exception.hpp>
#include <cpplinq/detail/information_schema/schema.hpp>
#include <cpplinq/detail/procedure_registry.hpp>
#include <cpplinq/detail/traits/underlying_column_type.hpp>
#include <tuple>
#include <vector>

#define DEFAULT_VALUE_AS_STRING(__parameter_tuple__)                       \
  BOOST_PP_IF(BOOST_PP_EQUAL(BOOST_PP_TUPLE_SIZE(__parameter_tuple__), 3), \
              BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM( \
                        BOOST_PP_TUPLE_SIZE(__parameter_tuple__), 2, __parameter_tuple__)),            \
                                 "")

#define DEFAULT_VALUE(__parameter_tuple__)                                  \
  BOOST_PP_IF(                                                              \
      BOOST_PP_EQUAL(BOOST_PP_TUPLE_SIZE(__parameter_tuple__), 3),          \
      type{BOOST_PP_TUPLE_ELEM(BOOST_PP_TUPLE_SIZE(__parameter_tuple__), 2, \
                               __parameter_tuple__)},                       \
      type{})

// __param_tuple__  = (name, type, [default])
#define DECLARE_PARAMETER(__ignored__, __procedure_name__, __I__,          \
                          __param_tuple__)                                 \
  template <>                                                              \
  struct parameter_trait<cpplinq::detail::traits::hash(                    \
      BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(BOOST_PP_TUPLE_SIZE(__param_tuple__), 0, __param_tuple__))),       \
      cpplinq::detail::traits::hash(__procedure_name__)> {                 \
    constexpr static auto hash =                                           \
        cpplinq::detail::traits::hash(BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(BOOST_PP_TUPLE_SIZE(__param_tuple__), 0, __param_tuple__)));     \
    constexpr static auto procedure_hash =                                 \
        cpplinq::detail::traits::hash(__procedure_name__);                 \
    constexpr static auto position = size_t{__I__};                        \
    using type = BOOST_PP_TUPLE_ELEM(BOOST_PP_TUPLE_SIZE(__param_tuple__), \
                                     1,                                    \
                                     __param_tuple__);                     \
    static type default_value() {                                          \
      static const auto default_value_ = DEFAULT_VALUE(__param_tuple__);   \
      return default_value_;                                               \
    }                                                                      \
                                                                           \
    static const type from_string(const std::string& value) {              \
      if (value.empty()) {                                                 \
        return default_value();                                            \
      }                                                                    \
      return cpplinq::detail::traits::underlying_column_type<              \
          type>::from_string(value);                                       \
    }                                                                      \
    static const std::string& procedure_name() {                           \
      static const auto name_ = std::string{__procedure_name__};           \
      return name_;                                                        \
    }                                                                      \
    static const std::string& name() {                                     \
      static const auto name_ =                                            \
          std::string{BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(BOOST_PP_TUPLE_SIZE(__param_tuple__), 0, __param_tuple__))};   \
      return name_;                                                        \
    }                                                                      \
  };

// __procedure_tuple__ = (__procecdure_name__, __procedure_type__,
// __procedure_method__)
#define REGISTER_PARAMETER(__ignored__, __procedure_tuple__, __I__,          \
                           __parameter_tuple__)                              \
  static auto BOOST_PP_CAT(                                                  \
      registered_,                                                           \
      BOOST_PP_CAT(                                                          \
          BOOST_PP_TUPLE_ELEM(3, 1, __procedure_tuple__),                    \
          BOOST_PP_CAT(                                                      \
              _, BOOST_PP_CAT(                                               \
                     BOOST_PP_TUPLE_ELEM(3, 2, __procedure_tuple__),         \
                     BOOST_PP_CAT(                                           \
                         _, BOOST_PP_TUPLE_ELEM(                             \
                                BOOST_PP_TUPLE_SIZE(__parameter_tuple__), 0, \
                                __parameter_tuple__)))))) = []() {           \
    auto parameter_def = cpplinq::detail::information_schema::parameter{     \
        .specific_name = BOOST_PP_TUPLE_ELEM(3, 0, __procedure_tuple__),     \
        .ordinal_position = __I__,                                           \
        .parameter_name = BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM( \
            BOOST_PP_TUPLE_SIZE(__parameter_tuple__), 0, __parameter_tuple__)),   \
            .data_type = BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM( \
            BOOST_PP_TUPLE_SIZE(__parameter_tuple__), 1, __parameter_tuple__)),            \
                .parameter_default =                                         \
                    DEFAULT_VALUE_AS_STRING(__parameter_tuple__),            \
            };                                                               \
    cpplinq::detail::information_schema::parameters::instance().push_back(   \
        std::move(parameter_def));                                           \
    return true;                                                             \
  }();

#define CONVERT_PARAMETER(__ignored_1__, __ignored_2__, __I__,       \
                          __param_tuple__)                           \
  BOOST_PP_COMMA_IF(__I__)                                           \
  parameter_trait<cpplinq::detail::traits::hash(                     \
      BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(BOOST_PP_TUPLE_SIZE(__param_tuple__), 0, __param_tuple__))), \
      hash>::from_string(all_args[__I__].second)

#define GET_PARAMETER_NAME(__ignored_1__, __ignored_2__, __parameter_tuple__) \
  BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(BOOST_PP_TUPLE_SIZE(__parameter_tuple__), 0, __parameter_tuple__)),

#define REGISTER_PROCEDURE(__procedure_name__, __procedure_type__,          \
                           __procedure_method__)                            \
  static auto BOOST_PP_CAT(                                                 \
      registered_,                                                          \
      BOOST_PP_CAT(__procedure_type__,                                      \
                   BOOST_PP_CAT(_, __procedure_method__))) = []() {         \
    using trait =                                                           \
        procedure_trait<cpplinq::detail::traits::hash(__procedure_name__)>; \
    cpplinq::detail::procedure_registry::instance().add(                    \
        trait::name(), cpplinq::detail::traits::any_procedure{trait{}});    \
    auto procedure_def = cpplinq::detail::information_schema::routine{      \
        .routine_name = __procedure_name__};                                \
    cpplinq::detail::information_schema::routines::instance().push_back(    \
        std::move(procedure_def));                                          \
    return true;                                                            \
  }();

// __procedure_tuple__
//     = (procedure_type, procedure_name, ((param1, type1,
//     [default]))...((paramN, typeN, [default])))
#define DECLARE_PROCEDURE(__procedure_name__, __procedure_type__,             \
                          __procedure_method__, __parameters__)               \
  namespace cpplinq::detail::traits {                                         \
  BOOST_PP_SEQ_FOR_EACH_I(DECLARE_PARAMETER,                                  \
                          __procedure_name__,                                 \
                          __parameters__)                                     \
  template <>                                                                 \
  struct procedure_trait<cpplinq::detail::traits::hash(__procedure_name__)> { \
    using procedure_type = __procedure_type__;                                \
    static constexpr auto hash =                                              \
        cpplinq::detail::traits::hash(__procedure_name__);                    \
                                                                              \
    static const std::vector<std::string>& parameters() {                     \
      static const auto parameters_ = std::vector<std::string>{               \
          BOOST_PP_SEQ_FOR_EACH(GET_PARAMETER_NAME, _, __parameters__)};      \
      return parameters_;                                                     \
    }                                                                         \
    static const std::string& name() {                                        \
      static const auto name_ = std::string{__procedure_name__};              \
      return name_;                                                           \
    }                                                                         \
    static cpplinq::detail::cursor invoke(const procedure_arguments& args) {  \
      auto all_args = reconcile<procedure_trait<hash>>(args);                 \
      return __procedure_type__::instance().__procedure_method__(             \
          BOOST_PP_SEQ_FOR_EACH_I(CONVERT_PARAMETER, _, __parameters__));     \
    }                                                                         \
  };                                                                          \
  BOOST_PP_SEQ_FOR_EACH_I(REGISTER_PARAMETER,                                 \
                          (__procedure_name__,                                \
                           __procedure_type__,                                \
                           __procedure_method__),                             \
                          __parameters__)                                     \
  REGISTER_PROCEDURE(__procedure_name__,                                      \
                     __procedure_type__,                                      \
                     __procedure_method__)                                    \
  }

#define DECLARE_PROCEDURE_NO_PARAMETERS(                                      \
    __procedure_name__, __procedure_type__, __procedure_method__)             \
  namespace cpplinq::detail::traits {                                         \
  template <>                                                                 \
  struct procedure_trait<cpplinq::detail::traits::hash(__procedure_name__)> { \
    using procedure_type = __procedure_type__;                                \
    static constexpr auto hash =                                              \
        cpplinq::detail::traits::hash(__procedure_name__);                    \
                                                                              \
    static const std::vector<std::string>& parameters() {                     \
      static const auto parameters_ = std::vector<std::string>{};             \
      return parameters_;                                                     \
    }                                                                         \
    static const std::string& name() {                                        \
      static const auto name_ = std::string{__procedure_name__};              \
      return name_;                                                           \
    }                                                                         \
    static cpplinq::detail::cursor invoke(const procedure_arguments& args) {  \
      return __procedure_type__::instance().__procedure_method__();           \
    }                                                                         \
  };                                                                          \
  REGISTER_PROCEDURE(__procedure_name__,                                      \
                     __procedure_type__,                                      \
                     __procedure_method__)                                    \
  }

namespace cpplinq::detail::traits {

template <size_t Hash, size_t Procedure_hash>
struct parameter_trait;

template <size_t Hash>
struct procedure_trait;

using procedure_arguments = std::vector<std::pair<std::string, std::string>>;

// This can be done in the any_procedure type
template <typename Procedure_trait>
procedure_arguments reconcile(const procedure_arguments& args) {
  auto all_arguments = procedure_arguments{};
  for (const auto& arg : Procedure_trait::parameters()) {
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

}  // namespace cpplinq::detail::traits