#pragma once

// __index_tuple__ = (index_name, index_type, (column1)(column2)...(columnN))
// __user_defined_tuple__ = (table_trait, table_name)

#define GET_INDEX_COLUMN_NAME(__ignored_1__, __ignored_2__, __I__, \
                              __column_tuple__)                    \
  BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(2, 0, __column_tuple__)),

#define GENERATE_INDEX_INVOKE_BLOCK(__ignored_1__, __ignored_2__, __I__, \
                                    __column_tuple__)                    \
  std::get<__I__>(t) = underlying_column_type<BOOST_PP_TUPLE_ELEM(       \
      2, 1, __column_tuple__)>::from_string(v[__I__]);

#define INSTANTIATE_INDEX(__ignored__, __user_defined_tuple__,   \
                          __index_tuple__)                       \
  {BOOST_PP_TUPLE_ELEM(3, 0, __index_tuple__),                   \
   {BOOST_PP_TUPLE_ELEM(2, 0, __user_defined_tuple__){},         \
    index_trait<BOOST_PP_TUPLE_ELEM(3, 1, __index_tuple__),      \
                cpplinq::detail::traits::hash(                  \
                    BOOST_PP_TUPLE_ELEM(3, 0, __index_tuple__)), \
                BOOST_PP_TUPLE_ELEM(2, 0, __user_defined_tuple__)::hash>{}}},

#define DECLARE_INDEX(__ignored__, __user_defined_tuple__, __index_tuple__) \
  template <>                                                               \
  struct index_trait<BOOST_PP_TUPLE_ELEM(3, 1, __index_tuple__),            \
                     cpplinq::detail::traits::hash(                        \
                         BOOST_PP_TUPLE_ELEM(3, 0, __index_tuple__)),       \
                     BOOST_PP_TUPLE_ELEM(2, 0,                              \
                                         __user_defined_tuple__)::hash> {   \
    using index_type = BOOST_PP_TUPLE_ELEM(3, 1, __index_tuple__);          \
    using iterator_type = typename index_type::iterator;                    \
    using tuple_type = typename index_type::tuple_type;                     \
    static constexpr auto hash = cpplinq::detail::traits::hash(            \
        BOOST_PP_TUPLE_ELEM(3, 0, __index_tuple__));                        \
    using table_trait = BOOST_PP_TUPLE_ELEM(2, 0, __user_defined_tuple__);  \
                                                                            \
    static const std::string& name() {                                      \
      static const auto name_ =                                             \
          std::string{BOOST_PP_TUPLE_ELEM(3, 0, __index_tuple__)};          \
      return name_;                                                         \
    }                                                                       \
    static const std::vector<std::string>& columns() {                      \
      static const auto columns_ =                                          \
          std::vector<std::string>{BOOST_PP_SEQ_FOR_EACH_I(                 \
              GET_INDEX_COLUMN_NAME, _,                                     \
              BOOST_PP_TUPLE_ELEM(3, 2, __index_tuple__))};                 \
      return columns_;                                                      \
    }                                                                       \
    static typename index_type::iterator begin() {                          \
      return index_type::instance().begin();                                \
    }                                                                       \
    static typename index_type::iterator end() {                            \
      return index_type::instance().begin();                                \
    }                                                                       \
    static tuple_type to_tuple(const std::vector<std::string>& v) {         \
      if (v.size() != std::tuple_size<tuple_type>()) {                      \
        throw cpplinq::detail::cpplinq_exception(                          \
            "Input not the same size as tuple type.");                      \
      }                                                                     \
      auto t = tuple_type{};                                                \
      auto& columns_ = columns();                                           \
      BOOST_PP_SEQ_FOR_EACH_I(GENERATE_INDEX_INVOKE_BLOCK, _,               \
                              BOOST_PP_TUPLE_ELEM(3, 2, __index_tuple__))   \
      return t;                                                             \
    }                                                                       \
    static typename index_type::iterator lower_bound(const tuple_type& t) { \
      return index_type::instance().lower_bound(t);                         \
    }                                                                       \
    static typename index_type::iterator upper_bound(const tuple_type& t) { \
      return index_type::instance().upper_bound(t);                         \
    }                                                                       \
  };                                                                        \
  static auto BOOST_PP_CAT(                                                 \
      registered_, BOOST_PP_TUPLE_ELEM(3, 1, __index_tuple__)) = []() {     \
    return true;                                                            \
  }();

namespace cpplinq::detail::traits {

template <typename Index_type, size_t Hash, size_t Table_hash>
struct index_trait;

}