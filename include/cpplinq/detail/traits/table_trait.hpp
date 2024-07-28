#pragma once
#include <cpplinq/detail/column.hpp>
#include <cpplinq/detail/cursor.hpp>
#include <cpplinq/detail/operators/evaluate.hpp>
#include <cpplinq/detail/operators/expression_tree.hpp>
#include <cpplinq/detail/traits/any_index.hpp>
#include <cpplinq/detail/traits/column_trait.hpp>
#include <cpplinq/detail/traits/index_trait.hpp>
#include <cpplinq/detail/traits/underlying_column_type.hpp>
#include <cstddef>
#include <map>
#include <memory>
#include <optional>
#include <vector>

// template <typename Table_type, typename Record_type, size_t Hash>
// struct table_trait {
//     using table_type = Table_type;
//     using record_type = Record_type;
//     using hash = Hash;

//     static const std::string& name() { return "foobar_table"; }

//     static const std::vector<std::string>& columns() {
//         return {"id", "foo", "bar", "foobar"};
//     }

//     // Converts the string value to the column type.
//     static std::any from_string(const std::string& column_name, const
//     std::string& value) {
//         auto result = std::any{};
//         invoke(column_name, [&](const auto& trait) {
//             result =
//             underlying_column_type<decltype(trait)::column_type>::from_string(value);
//         });
//         return result;
//     }

//     // Converts the column value to a string.
//     static std::string to_string(const std::string& column_name, const
//     record_type& record) {
//         auto result = std::string{};
//         invoke(column_name, [&](const auto& trait) {
//             const auto& value = trait.value(record);
//             result =
//             underlying_column_type<decltype(trait)::column_type>::to_string(value);
//         });
//         return result;
//     }

//     static bool evaluate(const record_type& record, cconst
//     pplinq::detail::operators::expression_tree& expression) {
//         return evaluate<decltype(*this)>(record, expression);
//     }

//     // Determines the relationship between the column and the given value.
//     static
//     std::unordered_set<cpplinq::detail::operators::comparison_result>
//     evaluate(const std::string& column_name, const record_type& record,
//     const std::any& value) {
//         auto result =
//         std::unordered_set<cpplinq::detail::operators::comparison_result>{};
//         invoke(column_name, [&](const auto& trait) {
//             const auto& column_value = trait.value(record);
//             const auto& op_value =
//             std::any_cast<decltype(trait)::column_type&>(value);

//             if (column_value < op_value) {
//                 result.emplace(cpplinq::detail::operators::comparison_result::less_than);
//             } else if (column_value > op_value) {
//                 result.emplace(cpplinq::detail::operators::comparison_result::greater_than);
//             }

//             if (column_value == op_value) {
//                 result.emplace(cpplinq::detail::operators::comparison_result::equal);
//             } else {
//                 result.emplace(cpplinq::detail::operators::comparison_result::not_equal);
//             }
//         });
//         return result;
//     }

//     template <typename Functor>
//     static void invoke(const std::string& column_name, Functor f) {
//         if (column_name == "id") {
//             auto trait_instance = column_trait<hash("foobar_table"),
//             hash("id")>{}; f(trait_instance); return;
//         }
//         if (column_name == "foo") {
//             auto trait_instance = column_trait<hash("foobar_table"),
//             hash("foo")>{}; f(trait_instance); return;
//         }
//     }
// };

#define GET_COLUMN_NAME(__ignored_1__, __ignored_2__, __column_tuple__) \
  BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(2, 0, __column_tuple__)),

#define GENERATE_INVOKE_BLOCK(__ignored__, __table_name__, __column_tuple__) \
  if (column_name ==                                                         \
      BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(2, 0, __column_tuple__))) {     \
    auto trait_instance = column_trait<cpplinq::detail::traits::hash(       \
        BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(2, 0, __column_tuple__))),    \
        cpplinq::detail::traits::hash(__table_name__)>{};                   \
    f(trait_instance);                                                       \
    return;                                                                  \
  }

#define DECLARE_TABLE(__table_name__, __table_type__, __record_type__,         \
                      __columns__, __indices__)                                \
  namespace cpplinq::detail::traits {                                         \
  BOOST_PP_SEQ_FOR_EACH_I(DECLARE_COLUMN,                                      \
                          (__table_name__, __table_type__, __record_type__),   \
                          __columns__)                                         \
  template <>                                                                  \
  struct table_trait<__table_type__,                                           \
                     __record_type__,                                          \
                     cpplinq::detail::traits::hash(__table_name__)> {         \
    static constexpr auto hash =                                               \
        cpplinq::detail::traits::hash(__table_name__);                        \
    using table_type = __table_type__;                                         \
    using record_type = __record_type__;                                       \
    using type = table_trait<__table_type__,                                   \
                             __record_type__,                                  \
                             cpplinq::detail::traits::hash(__table_name__)>;  \
                                                                               \
    static const std::string& name() {                                         \
      static const auto name = std::string{__table_name__};                    \
      return name;                                                             \
    }                                                                          \
    static const std::vector<std::string>& columns() {                         \
      static const auto columns_ = std::vector<std::string>{                   \
          BOOST_PP_SEQ_FOR_EACH(GET_COLUMN_NAME, _, __columns__)};             \
      return columns_;                                                         \
    }                                                                          \
    static any_index index_for(const std::string& name);                       \
    template <typename Functor>                                                \
    static void invoke(const std::string& column_name, Functor f) {            \
      BOOST_PP_SEQ_FOR_EACH(GENERATE_INVOKE_BLOCK, __table_name__,             \
                            __columns__)                                       \
    }                                                                          \
    static std::any column_value(const std::string& column_name,               \
                                 const record_type& record) {                  \
      auto result = std::any{};                                                \
      invoke(column_name,                                                      \
             [&](const auto& trait) { result = trait.value(record); });        \
      return result;                                                           \
    }                                                                          \
    static std::any from_string(const std::string& column_name,                \
                                const std::string& value) {                    \
      auto result = std::any{};                                                \
      invoke(column_name, [&](const auto& trait) {                             \
        result = underlying_column_type<typename std::decay_t<                 \
            decltype(trait)>::column_type>::from_string(value);                \
      });                                                                      \
      return result;                                                           \
    }                                                                          \
    static std::string to_string(const std::string& column_name,               \
                                 const record_type& record) {                  \
      auto result = std::string{};                                             \
      invoke(column_name, [&](const auto& trait) {                             \
        const auto& value = trait.value(record);                               \
        result = underlying_column_type<typename std::decay_t<                 \
            decltype(trait)>::column_type>::to_string(value);                  \
      });                                                                      \
      return result;                                                           \
    }                                                                          \
    static bool evaluate(                                                      \
        const record_type& record,                                             \
        cpplinq::detail::operators::expression_tree& expression) {            \
      return cpplinq::detail::operators::evaluate<type>(record, expression);  \
    }                                                                          \
    static std::unordered_set<cpplinq::detail::operators::comparison_result>  \
    evaluate(const std::string& column_name,                                   \
             const record_type& record,                                        \
             const std::any& value) {                                          \
      auto result = std::unordered_set<                                        \
          cpplinq::detail::operators::comparison_result>{};                   \
      invoke(column_name, [&](const auto& trait) {                             \
        const auto& column_value = trait.value(record);                        \
        const auto& op_value = std::any_cast<                                  \
            const typename std::decay_t<decltype(trait)>::column_type&>(       \
            value);                                                            \
        result =                                                               \
            cpplinq::detail::operators::evaluate(column_value, op_value);     \
      });                                                                      \
      return result;                                                           \
    }                                                                          \
  };                                                                           \
  static auto BOOST_PP_CAT(registered_, __table_type__) = []() {               \
    using trait = table_trait<__table_type__, __record_type__,                 \
                              cpplinq::detail::traits::hash(__table_name__)>; \
    cpplinq::detail::table_registry::instance().add(                          \
        __table_name__, cpplinq::detail::traits::any_table{trait{}});         \
    auto table_def = cpplinq::detail::information_schema::table{              \
        .table_name = __table_name__,                                          \
    };                                                                         \
    cpplinq::detail::information_schema::tables::instance().push(             \
        std::move(table_def));                                                 \
    return true;                                                               \
  }();                                                                         \
  using BOOST_PP_CAT(__table_type__, _table_trait) =                           \
      table_trait<__table_type__,                                              \
                  __record_type__,                                             \
                  cpplinq::detail::traits::hash(__table_name__)>;             \
  BOOST_PP_SEQ_FOR_EACH(DECLARE_INDEX,                                         \
                        (BOOST_PP_CAT(__table_type__, _table_trait),           \
                         __table_name__),                                      \
                        __indices__)                                           \
  any_index BOOST_PP_CAT(__table_type__,                                       \
                         _table_trait)::index_for(const std::string& name) {   \
    const static auto indices =                                                \
        std::map<std::string, any_index>{BOOST_PP_SEQ_FOR_EACH(                \
            INSTANTIATE_INDEX,                                                 \
            (BOOST_PP_CAT(__table_type__, _table_trait), __table_name__),      \
            __indices__)};                                                     \
    if (indices.empty()) {                                                     \
      throw cpplinq_exception("Need at least one index defined");              \
    }                                                                          \
    if (name.empty()) {                                                        \
      return std::begin(indices)->second;                                      \
    }                                                                          \
    auto it = indices.find(name);                                              \
    if (it == std::end(indices)) {                                             \
      throw cpplinq_exception{"Unknown index"};                                \
    }                                                                          \
    return it->second;                                                         \
  }                                                                            \
  }  // namespace cpplinq::detail::traits

namespace cpplinq::detail::traits {

template <typename Table_type, typename Record_type, size_t Hash>
struct table_trait;

}  // namespace cpplinq::detail::traits
