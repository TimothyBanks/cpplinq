#pragma once
#include <boost/preprocessor.hpp>
#include <cpplinq/detail/information_schema/schema.hpp>
#include <cpplinq/detail/traits/hash.hpp>

// template <size_t Hash, size_t Table_hash>
// struct column_trait {
//     using hash = Hash;
//     using table_hash = Table_hash;
//     using column_type = size_t; // This will be stamped out by the macro

//     static const auto& name() { return "id"; }
//     static auto& value(const record_type& r) {
//         return r.id; // Will be stamped out by the macro.
//     }
// };

#define DECLARE_COLUMN(__ignored__, __user_defined_tuple__, __I__,             \
                       __column_tuple__)                                       \
  template <>                                                                  \
  struct column_trait<cpplinq::detail::traits::hash(                           \
      BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(2, 0, __column_tuple__))),          \
      cpplinq::detail::traits::hash(                                           \
          BOOST_PP_TUPLE_ELEM(3, 0, __user_defined_tuple__))> {                \
    static constexpr auto hash = cpplinq::detail::traits::hash(                \
        BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(2, 0, __column_tuple__)));        \
    static constexpr auto table_hash = cpplinq::detail::traits::hash(          \
        BOOST_PP_TUPLE_ELEM(3, 0, __user_defined_tuple__));                    \
    using column_type = BOOST_PP_TUPLE_ELEM(2, 1, __column_tuple__);           \
                                                                               \
    static const auto& name() {                                                \
      return BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(2, 0, __column_tuple__));    \
    }                                                                          \
    static auto& value(                                                        \
        const BOOST_PP_TUPLE_ELEM(3, 2, __user_defined_tuple__) & r) {         \
      return r.BOOST_PP_TUPLE_ELEM(2, 0, __column_tuple__);                    \
    }                                                                          \
    static void set_value(BOOST_PP_TUPLE_ELEM(3, 2, __user_defined_tuple__) &  \
                              r,                                               \
                          const std::string& value) {                          \
      r.BOOST_PP_TUPLE_ELEM(2, 0, __column_tuple__) =                          \
          underlying_column_type<column_type>::from_string(value);             \
    }                                                                          \
  };                                                                           \
  static auto BOOST_PP_CAT(                                                    \
      registered_,                                                             \
      BOOST_PP_CAT(BOOST_PP_TUPLE_ELEM(3, 1, __user_defined_tuple__),          \
                   BOOST_PP_CAT(_, BOOST_PP_TUPLE_ELEM(                        \
                                       2, 0, __column_tuple__)))) = []() {     \
    auto column_def = cpplinq::detail::information_schema::column{             \
        .table_name = BOOST_PP_TUPLE_ELEM(3, 0, __user_defined_tuple__),       \
        .column_name = BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(2, 0, __column_tuple__)),                        \
            .ordinal_position = __I__,                                         \
            .data_type =                                                       \
                BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(2, 1, __column_tuple__)), \
            };                                                                 \
    cpplinq::detail::information_schema::columns::instance().push_back(        \
        std::move(column_def));                                                \
    return true;                                                               \
  }();

namespace cpplinq::detail::traits {

template <size_t Hash, size_t Table_hash>
struct column_trait;

}  // namespace cpplinq::detail::traits