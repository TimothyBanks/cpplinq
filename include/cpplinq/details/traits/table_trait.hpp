#pragma once
#include <cpplinq/details/column.hpp>
#include <cpplinq/details/cursor.hpp>
#include <cpplinq/details/operators/evaluate.hpp>
#include <cpplinq/details/operators/expression_tree.hpp>
#include <cstddef>
#include <memory>
#include <optional>
#include <vector>

namespace cpplinq::details::traits {
constexpr size_t hash(const char *str, size_t hash_value = 0xcbf29ce484222325) {
  return (*str == '\0')
             ? hash_value
             : hash(str + 1,
                    (hash_value ^ static_cast<size_t>(*str)) * 0x100000001b3);
}

constexpr size_t hash(const std::string &str) { return hash(str.c_str()); }
} // namespace cpplinq::details::traits

struct any_column {
private:
  struct column_base {
    virtual const std::string &name() = 0;
  };

  template <typename Column_trait> struct column : public column_base {
    using column_trait = Column_trait;
  };

  std::shared_ptr<column_base> ptr;

public:
  any_column() = default;
  any_column(const any_column &) = default;
  any_column(any_column &&) = default;
  template <typename Column_trait> any_column(std::nullptr_t);

  any_column &operator=(const any_column &) = default;
  any_column &operator=(any_column &&) = default;
};

struct any_index {
private:
  struct index_base {};

  template <typename Index_trait> struct index : public index_base {
    using index_trait = Index_trait;
  };

  std::shared_ptr<index_base> ptr;

public:
  any_index() = default;
  any_index(const any_index &) = default;
  any_index(any_index &&) = default;
  template <typename Column_trait> any_index(std::nullptr_t);

  any_index &operator=(const any_index &) = default;
  any_index &operator=(any_index &&) = default;
};

template <typename T> struct underlying_column_type {
  T from_string(const std::string &s);
  std::string to_string(const T &t);
  std::string to_string(const std::any &a);
};

template <size_t Hash, size_t Table_hash> struct column_trait;

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

#define DECLARE_COLUMN(__ignored__, __user_defined_tuple__, __column_tuple__)  \
  template <>                                                                  \
  struct column_trait<cpplinq::details::traits::hash(                          \
                          BOOST_PP_TUPLE_ELEM(2, 0, __column_tuple__)),        \
                      cpplinq::details::traits::hash(BOOST_PP_TUPLE_ELEM(      \
                          2, 0, __user_defined_tuple__))> {                    \
    static constexpr auto hash = cpplinq::details::traits::hash(               \
        BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(2, 0, __column_tuple__)));      \
    static constexpr auto table_hash = cpplinq::details::traits::hash(         \
        BOOST_PP_TUPLE_ELEM(2, 0, __user_defined_tuple__));                    \
    using column_type = BOOST_PP_TUPLE_ELEM(2, 1, __column_tuple__);           \
                                                                               \
    static const auto &name() {                                                \
      return BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(2, 0, __column_tuple__));  \
    }                                                                          \
    \  
    static auto &                                                              \
    value(const BOOST_PP_TUPLE_ELEM(2, 1, __user_defined_tuple__) & r) {       \
      return r.BOOST_PP_TUPLE_ELEM(2, 0, __column_tuple__);                    \
    }                                                                          \
  };                                                                           \
  static auto BOOST_PP_CAT(                                                    \
      registered_,                                                             \
      BOOST_PP_CAT(                                                            \
          BOOST_PP_TUPLE_ELEM(2, 0, __user_defined_tuple__),                   \
          BOOST_PP_CAT(_, BOOST_PP_TUPLE_ELEM(2, 0, __column_tuple__)))) =     \
      []() { return true; }();

#define GET_COLUMN_NAME(__ignored_1__, __ignored_2__, __column_tuple__)        \
  BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(2, 0, __column_tuple__)),

#define GENERATE_INVOKE_BLOCK(__ignored__, __table_name__, __column_tuple__)   \
  if (column_name ==                                                           \
      BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(2, 0, __column_tuple))) {         \
    auto trait_instance = column_trait<cpplinq::details::traits::hash(         \
        BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(2, 0, __column_tuple))),        \
        cpplinq::details::traits::hash(__table_name__)>{};                     \
    f(trait_instance);                                                         \
    return;                                                                    \
  }

template <typename Table_type, typename Record_type, size_t Hash>
struct table_trait;

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
//     static std::unordered_set<cpplinq::details::operators::comparison_result>
//     evaluate(const std::string& column_name, const record_type& record, const
//     std::any& value) {
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

template <typename T, typename U = T>
std::vector<cpplinq::details::operators::comparison_result>
evaluate(const T &left, const U &right) {
  auto result = std::vector<cpplinq::details::operators::comparison_result>{};
  if (left < right) {
    result.emplace(cpplinq::details::operators::comparison_result::less_than);
  } else if (left > right) {
    result.emplace(
        cpplinq::details::operators::comparison_result::greater_than);
  }
  if (left == right) {
    result.emplace(cpplinq::details::operators::comparison_result::equal);
  } else {
    result.emplace(cpplinq::details::operators::comparison_result::not_equal);
  }
  return result;
}

#define DECLARE_TABLE(__table_name__, __table_type__, __record_type__,         \
                      __columns__, __indices__)                                \
  BOOST_PP_SEQ_FOR_EACH(__columns__, (__table_name__, __record_type__),        \
                        DECLARE_COLUMN);                                       \
  template <>                                                                  \
  struct table_trait<__table_type__, __record_type__,                          \
                     cpplinq::details::traits::hash(__table_name__)> {         \
    static constexpr auto hash =                                               \
        cpplinq::details::traits::hash(__table_name__);                        \
    using table_type = __table_type__;                                         \
    using record_type = __record_type__;                                       \
                                                                               \
    static const std::string &name() { return __table_name__; }                \
    static const std::vector<std::string> &columns() {                         \
      static const auto columns_ = std::vector<std::string>{                   \
          BOOST_PP_SEQ_FOR_EACH(GET_COLUMN_NAME, _, __columns__)};             \
      return columns_;                                                         \
    }                                                                          \
    template <typename Functor>                                                \
    static void invoke(const std::string &column_name, Functor f) {            \
      BOOST_PP_SEQ_FOR_EACH(GENERATE_INVOKE_BLOCK, __table_name__, __columns)  \
    }                                                                          \
    static std::any from_string(const std::string &column_name,                \
                                const std::string &value) {                    \
      auto result = std::any{};                                                \
      invoke(column_name, [&](const auto &trait) {                             \
        result = underlying_column_type_t<declytpe(                            \
            trait)::column_type>::from_string(value);                          \
      });                                                                      \
      return result;                                                           \
    }                                                                          \
    static std::string to_string(const std::string &column_name,               \
                                 const record_type &record) {                  \
      auto result = std::string{};                                             \
      invoke(column_name, [&](const auto &trait) {                             \
        const auto &value = trait.value(record);                               \
        result =                                                               \
            underlying_column_type_t<decltype(trait)::column_type>::to_string( \
                value);                                                        \
      });                                                                      \
      return result;                                                           \
    }                                                                          \
    static bool                                                                \
    evaluate(const record_type &record,                                        \
             cconst pplinq::details::operators::expression_tree &expression) { \
      return evaluate<decltype(*this)>(record, expression);                    \
    }                                                                          \
    static std::unordered_set<cpplinq::details::operators::comparison_result>  \
    evaluate(const std::string &column_name, const record_type &record,        \
             const std::any &value) {                                          \
      auto result = std::unordered_set<                                        \
          cpplinq::details::operators::comparison_result>{};                   \
      invoke(column_name, [&](const auto &trait) {                             \
        const auto &column_value = trait.value(record);                        \
        const auto &op_value =                                                 \
            std::any_cast<decltype(trait)::column_type &>(value);              \
        result = evaluate(column_value, op_value);                             \
      });                                                                      \
      return result;                                                           \
    }                                                                          \
  };                                                                           \
  static auto BOOST_PP_CAT(registered_, __table_name__) = []() {               \
    return true;                                                               \
  }();

template <typename Table_trait>
cpplinq::details::cursor execute(const select_context &context) {
  auto &primary_index = Table_trait::table_type::instance().primary_index();

  for (auto it = std::begin(primary_index); it != std::end(primary_index);
       ++it) {
  }
}

struct any_table {
public:
private:
  struct table_base {
    virtual const std::vector<any_column> &columns() = 0;
    virtual const std::vector<any_index> &index() = 0;
    virtual cpplinq::details::cursor execute(const select_info &info) const = 0;
  };

  template <typename Table_trait> struct table : public table_base {
    using table_trait = Table_trait;
  };

  std::shared_ptr<table_base> ptr;

public:
  any_table() = default;
  any_table(const any_table &) = default;
  any_table(any_table &&) = default;

  template <typename Table_trait> any_table(std::nullptr_t);

  any_table &operator=(const any_table &) = default;
  any_table &operator=(any_table &&) = default;

  const std::vector<any_column> &columns() const;
  const std::vector<any_index> &indices() const;

  cpplinq::details::cursor execute(const select_info &info) const;
};
