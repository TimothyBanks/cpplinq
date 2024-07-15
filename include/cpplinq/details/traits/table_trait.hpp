#pragma once
#include <cpplinq/details/column.hpp>
#include <cpplinq/details/cursor.hpp>
#include <cpplinq/details/operators/evaluate.hpp>
#include <cpplinq/details/operators/expression_tree.hpp>
#include <cpplinq/details/traits/column_trait.hpp>
#include <cstddef>
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
//             underlying_column_type_t<declytpe(trait)::column_type>::from_string(value);
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
//             underlying_column_type_t<decltype(trait)::column_type>::to_string(value);
//         });
//         return result;
//     }

//     static bool evaluate(const record_type& record, cconst
//     pplinq::details::operators::expression_tree& expression) {
//         return evaluate<decltype(*this)>(record, expression);
//     }

//     // Determines the relationship between the column and the given value.
//     static
//     std::unordered_set<cpplinq::details::operators::comparison_result>
//     evaluate(const std::string& column_name, const record_type& record,
//     const std::any& value) {
//         auto result =
//         std::unordered_set<cpplinq::details::operators::comparison_result>{};
//         invoke(column_name, [&](const auto& trait) {
//             const auto& column_value = trait.value(record);
//             const auto& op_value =
//             std::any_cast<decltype(trait)::column_type&>(value);

//             if (column_value < op_value) {
//                 result.emplace(cpplinq::details::operators::comparison_result::less_than);
//             } else if (column_value > op_value) {
//                 result.emplace(cpplinq::details::operators::comparison_result::greater_than);
//             }

//             if (column_value == op_value) {
//                 result.emplace(cpplinq::details::operators::comparison_result::equal);
//             } else {
//                 result.emplace(cpplinq::details::operators::comparison_result::not_equal);
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
      BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(2, 0, __column_tuple))) {       \
    auto trait_instance = column_trait<cpplinq::details::traits::hash(       \
        BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(2, 0, __column_tuple))),      \
        cpplinq::details::traits::hash(__table_name__)>{};                   \
    f(trait_instance);                                                       \
    return;                                                                  \
  }

#define DECLARE_TABLE(__table_name__, __table_type__, __record_type__,         \
                      __columns__, __indices__)                                \
  namespace cpplinq::details::traits {                                         \
  BOOST_PP_SEQ_FOR_EACH(__columns__,                                           \
                        (__table_name__, __record_type__),                     \
                        DECLARE_COLUMN);                                       \
  template <>                                                                  \
  struct table_trait<__table_type__,                                           \
                     __record_type__,                                          \
                     cpplinq::details::traits::hash(__table_name__)> {         \
    static constexpr auto hash =                                               \
        cpplinq::details::traits::hash(__table_name__);                        \
    using table_type = __table_type__;                                         \
    using record_type = __record_type__;                                       \
                                                                               \
    static const std::string& name() { return __table_name__; }                \
    static const std::vector<std::string>& columns() {                         \
      static const auto columns_ = std::vector<std::string>{                   \
          BOOST_PP_SEQ_FOR_EACH(GET_COLUMN_NAME, _, __columns__)};             \
      return columns_;                                                         \
    }                                                                          \
    template <typename Functor>                                                \
    static void invoke(const std::string& column_name, Functor f) {            \
      BOOST_PP_SEQ_FOR_EACH(GENERATE_INVOKE_BLOCK, __table_name__, __columns)  \
    }                                                                          \
    static std::any from_string(const std::string& column_name,                \
                                const std::string& value) {                    \
      auto result = std::any{};                                                \
      invoke(column_name, [&](const auto& trait) {                             \
        result = underlying_column_type_t<declytpe(                            \
            trait)::column_type>::from_string(value);                          \
      });                                                                      \
      return result;                                                           \
    }                                                                          \
    static std::string to_string(const std::string& column_name,               \
                                 const record_type& record) {                  \
      auto result = std::string{};                                             \
      invoke(column_name, [&](const auto& trait) {                             \
        const auto& value = trait.value(record);                               \
        result =                                                               \
            underlying_column_type_t<decltype(trait)::column_type>::to_string( \
                value);                                                        \
      });                                                                      \
      return result;                                                           \
    }                                                                          \
    static bool evaluate(                                                      \
        const record_type& record,                                             \
        cconst pplinq::details::operators::expression_tree& expression) {      \
      return evaluate<decltype(*this)>(record, expression);                    \
    }                                                                          \
    static std::unordered_set<cpplinq::details::operators::comparison_result>  \
    evaluate(const std::string& column_name,                                   \
             const record_type& record,                                        \
             const std::any& value) {                                          \
      auto result = std::unordered_set<                                        \
          cpplinq::details::operators::comparison_result>{};                   \
      invoke(column_name, [&](const auto& trait) {                             \
        const auto& column_value = trait.value(record);                        \
        const auto& op_value =                                                 \
            std::any_cast<decltype(trait)::column_type&>(value);               \
        result = evaluate(column_value, op_value);                             \
      });                                                                      \
      return result;                                                           \
    }                                                                          \
  };                                                                           \
  static auto BOOST_PP_CAT(registered_, __table_name__) = []() {               \
    using trait = table_trait<__table_type__, __record_type__,                 \
                              cpplinq::details::traits::hash(__table_name__)>; \
    cpplinq::details::table_registry::instance().add(__table_name__,           \
                                                     any_table<trait>{});      \
    return true;                                                               \
  }();                                                                         \
  }  // namespace cpplinq::details::traits

namespace cpplinq::details::traits {

template <typename Table_type, typename Record_type, size_t Hash>
struct table_trait;

}  // namespace cpplinq::details::traits
