# cpplinq
Library for overlaying an SQL syntax onto cpp data structures.  This will allow for executing various SQL statements against your data structures such as SELECT, INSERT, CALL.  This library will follow a postgres flavor of SQL.

The concept of this library came about as a possible way to white-box blockchain data.  While blockchains are known for transaparency, getting to meaningful data can be hard to do.  This is also coupled with people might not be familiar with toolchains, APIs, languages, etc that are used with the blockchain that is holding the data.  This is why SQL was choosen as the facade, because most people are comfortable with that toolset and language.  This also allows for future enhancements such as extending the library to mimic a DBMS and conforming to ODBC/JDBC standards, which would allow the blockchain to be integrated into your system as another SQL based data source.

For now, it is much easier to show an example of the library to demonstrate how to use and hopefully give an understanding of how it could be used.  The are a few major concepts that this library is employing:
1.  Duck typing.  There is an implicit interface that this library requires for integrators, but this interface can be exposed on your existing types with minimal changes.  One of the great features of CPP templates is duck typing, "if it walks like a duck and talks like a duck, it's a duck", or in our context, if the type has a method named the same as the method required by the API and matches the function signature, then it must be a type compatible with us.
2.  Type traits.  This is a solid design pattern globally accepted by the CPP community that allows encapsulation of most of this information needed by cpplinq.  This nice thing about this is that the information is also available at compile time.
3.  Macros.  Most of this library is built on the concept of code generation and that is exactly what Macros give us.
4. Type erasure.  Because who needs type information anyways.

Additional features that could be looked into is the use of an LLVM and clang compiler plugin that could generate this code at compile time using simple annotations on your types.

## DECLARE_TABLE
To declare and expose tables through this API, you can refer to the following examples.

```
// cpplinq places no concepts on the table structure.
struct foo_record {
    uint64_t id;
    std::string foo;
    float bar;
    std::vector<std::string> foobar;
};

// cpplinq utilizes a concept called duck typing.
// When it is working against a declared table, at certain
// points it will request a table "index" for iterating over.
// The type returned must conform to the range concept.  This
// type can be whatever you want it to be as long as it conforms
// to that concept.
template <typename Table, typename... T>
struct table_index {
  using tuple_type = std::tuple<T...>;
  using table_type = Table;
  using backing_type = std::map<tuple_type, size_t>;

  backing_type index_;
  table_type* table_;

  static table_index& instance() {
    static auto instance_ = table_index{};
    return instance_;
  }

  void table(table_type& t) { table_ = &t; }

  table_index() = default;
  table_index(const table_index&) = default;
  table_index(table_index&&) = default;

  table_index(table_type& table) : table_{&table} {}

  table_index& operator=(const table_index&) = default;
  table_index& operator=(table_index&&) = default;

  void push(tuple_type key, size_t position) {
    index_.emplace(std::move(key), position);
  }

  void pop(const tuple_type& key) { index_.erase(key); }

  struct iterator {
    typename backing_type::iterator begin_;
    typename backing_type::iterator end_;
    mutable typename backing_type::iterator it_;
    table_type* table_;

    iterator() = default;
    iterator(const iterator&) = default;
    iterator(iterator&&) = default;

    iterator(table_type& table,
             typename backing_type::iterator begin,
             typename backing_type::iterator end,
             typename backing_type::iterator it)
        : table_{&table},
          begin_{std::move(begin)},
          end_{std::move(end)},
          it_{std::move(it)} {}

    iterator& operator=(const iterator&) = default;
    iterator& operator=(iterator&&) = default;

    typename table_type::record_type& operator*() {
      auto position = it_->second;
      if (position >= table_->data().size()) {
        throw "iterator out of bounds";
      }
      return table_->data()[position];
    }

    const typename table_type::record_type& operator*() const {
      auto position = it_->second;
      if (position >= table_->data().size()) {
        throw "iterator out of bounds";
      }
      return *(table_->data()[position]);
    }

    iterator& operator++() {
      ++it_;
      return *this;
    }
    iterator& operator--() {
      --it_;
      return *this;
    }
    const iterator& operator++() const {
      ++it_;
      return *this;
    }
    const iterator& operator--() const {
      --it_;
      return *this;
    }

    bool operator==(const iterator& other) const { return it_ == other.it_; }
    bool operator!=(const iterator& other) const { return it_ != other.it_; }
  };

  iterator begin() {
    return iterator{*table_, std::begin(index_), std::end(index_),
                    std::begin(index_)};
  }

  iterator end() {
    return iterator{*table_, std::begin(index_), std::end(index_),
                    std::end(index_)};
  }

  iterator lower_bound(const tuple_type& key) {
    return iterator{*table_, std::begin(index_), std::end(index_),
                    index_.lower_bound(key)};
  }

  iterator upper_bound(const tuple_type& key) {
    return iterator{*table_, std::begin(index_), std::end(index_),
                    index_.upper_bound(key)};
  }

  const iterator begin() const {
    return iterator{*table_, std::begin(index_), std::end(index_),
                    std::begin(index_)};
  }

  const iterator end() const {
    return iterator{*table_, std::begin(index_), std::end(index_),
                    std::end(index_)};
  }

  const iterator lower_bound(const tuple_type& key) const {
    return iterator{*table_, std::begin(index_), std::end(index_),
                    index_.lower_bound(key)};
  }

  const iterator upper_bound(const tuple_type& key) const {
    return iterator{*table_, std::begin(index_), std::end(index_),
                    index_.upper_bound(key)};
  }
};

// cpplinq utilizes a concept called duck typing.
// For a table, the following concepts are expected:
// 1.  It is a singleton and exposes a static method called instance.
// 2.  It has an instance method, called primary_index, to retrieve the primary index to iterate on.
// 3.  The "index" type returned must conform to the range concept.
// 4.  As the development of this library progresses, there may be additional concepts centered around secondary indices.
struct foo_table {
  friend class table_index<foo_table, size_t>;

  using record_type = foo_record;
  using backing_store = std::vector<record_type>;
  using primary_index_type = table_index<foo_table, size_t>;

  backing_store records_;

  foo_table() { primary_index_type::instance().table(*this); }

  foo_table(const foo_table&) = default;
  foo_table(foo_table&&) = default;

  foo_table& operator=(const foo_table&) = default;
  foo_table& operator=(foo_table&&) = default;

  static foo_table& instance() {
    static auto instance_ = foo_table{};
    return instance_;
  }

  void push(record_type record) {
    records_.emplace_back();
    records_.back() = std::move(record);
    primary_index_type::instance().push(std::make_tuple(record.id),
                                        records_.size() - 1);
  }

  void pop(size_t index) {
    if (index >= records_.size()) {
      return;
    }
    primary_index_type::instance().pop(std::make_tuple(records_[index].id));
    records_.erase(std::begin(records_) + index);
  }

  backing_store& data() { return records_; }
  const backing_store& data() const { return records_; }

  const primary_index_type& primary_index() const {
    return primary_index_type::instance();
  }

  primary_index_type& primary_index() { return primary_index_type::instance(); }
};

// This makes the table known to cpplinq.
DECLARE_TABLE("foo_table",
              foo_table,
              foo_record,
              ((id, size_t))((foo, std::string))((bar, float))(
                  (foobar, std::vector<std::string>)),
              (("id", foo_table_index_1, ((id, size_t)))));

...

// Populate your table.
...

// Then as a simple example of how this could be used
auto cursor = cpplinq::sql_context::execute("SELECT foo, bar, foobar FROM foo_table WHERE id > 100 AND id < 200;");

```

In regards to the `DECLARE_TABLE` macro, it will expand to something like this:

```
namespace cpplinq::details::traits {
template <>
struct column_trait<cpplinq::details::traits::hash("id"),
                    cpplinq::details::traits::hash("foo_table")> {
  static constexpr auto hash = cpplinq::details::traits::hash("id");
  static constexpr auto table_hash =
      cpplinq::details::traits::hash("foo_table");
  using column_type = size_t;
  static const auto& name() { return "id"; }
  static auto& value(const foo_record& r) { return r.id; }
};
static auto registered_foo_table_id = []() { return true; }();
template <>
struct column_trait<cpplinq::details::traits::hash("foo"),
                    cpplinq::details::traits::hash("foo_table")> {
  static constexpr auto hash = cpplinq::details::traits::hash("foo");
  static constexpr auto table_hash =
      cpplinq::details::traits::hash("foo_table");
  using column_type = std::string;
  static const auto& name() { return "foo"; }
  static auto& value(const foo_record& r) { return r.foo; }
};
static auto registered_foo_table_foo = []() { return true; }();
template <>
struct column_trait<cpplinq::details::traits::hash("bar"),
                    cpplinq::details::traits::hash("foo_table")> {
  static constexpr auto hash = cpplinq::details::traits::hash("bar");
  static constexpr auto table_hash =
      cpplinq::details::traits::hash("foo_table");
  using column_type = float;
  static const auto& name() { return "bar"; }
  static auto& value(const foo_record& r) { return r.bar; }
};
static auto registered_foo_table_bar = []() { return true; }();
template <>
struct column_trait<cpplinq::details::traits::hash("foobar"),
                    cpplinq::details::traits::hash("foo_table")> {
  static constexpr auto hash = cpplinq::details::traits::hash("foobar");
  static constexpr auto table_hash =
      cpplinq::details::traits::hash("foo_table");
  using column_type = std::vector<std::string>;
  static const auto& name() { return "foobar"; }
  static auto& value(const foo_record& r) { return r.foobar; }
};
static auto registered_foo_table_foobar = []() { return true; }();
template <>
struct table_trait<foo_table,
                   foo_record,
                   cpplinq::details::traits::hash("foo_table")> {
  static constexpr auto hash = cpplinq::details::traits::hash("foo_table");
  using table_type = foo_table;
  using record_type = foo_record;
  using type = table_trait<foo_table,
                           foo_record,
                           cpplinq::details::traits::hash("foo_table")>;
  static const std::string& name() {
    static const auto name = std::string{"foo_table"};
    return name;
  }
  static const std::vector<std::string>& columns() {
    static const auto columns_ = std::vector<std::string>{
        "id",
        "foo",
        "bar",
        "foobar",
    };
    return columns_;
  }
  static any_index index_for(const std::string& name);
  template <typename Functor>
  static void invoke(const std::string& column_name, Functor f) {
    if (column_name == "id") {
      auto trait_instance =
          column_trait<cpplinq::details::traits::hash("id"),
                       cpplinq::details::traits::hash("foo_table")>{};
      f(trait_instance);
      return;
    }
    if (column_name == "foo") {
      auto trait_instance =
          column_trait<cpplinq::details::traits::hash("foo"),
                       cpplinq::details::traits::hash("foo_table")>{};
      f(trait_instance);
      return;
    }
    if (column_name == "bar") {
      auto trait_instance =
          column_trait<cpplinq::details::traits::hash("bar"),
                       cpplinq::details::traits::hash("foo_table")>{};
      f(trait_instance);
      return;
    }
    if (column_name == "foobar") {
      auto trait_instance =
          column_trait<cpplinq::details::traits::hash("foobar"),
                       cpplinq::details::traits::hash("foo_table")>{};
      f(trait_instance);
      return;
    }
  }
  static std::any column_value(const std::string& column_name,
                               const record_type& record) {
    auto result = std::any{};
    invoke(column_name,
           [&](const auto& trait) { result = trait.value(record); });
    return result;
  }
  static std::any from_string(const std::string& column_name,
                              const std::string& value) {
    auto result = std::any{};
    invoke(column_name, [&](const auto& trait) {
      result = underlying_column_type<typename std::decay_t<
          decltype(trait)>::column_type>::from_string(value);
    });
    return result;
  }
  static std::string to_string(const std::string& column_name,
                               const record_type& record) {
    auto result = std::string{};
    invoke(column_name, [&](const auto& trait) {
      const auto& value = trait.value(record);
      result = underlying_column_type<typename std::decay_t<
          decltype(trait)>::column_type>::to_string(value);
    });
    return result;
  }
  static bool evaluate(
      const record_type& record,
      cpplinq::details::operators::expression_tree& expression) {
    return cpplinq::details::operators::evaluate<type>(record, expression);
  }
  static std::unordered_set<cpplinq::details::operators::comparison_result>
  evaluate(const std::string& column_name,
           const record_type& record,
           const std::any& value) {
    auto result =
        std::unordered_set<cpplinq::details::operators::comparison_result>{};
    invoke(column_name, [&](const auto& trait) {
      const auto& column_value = trait.value(record);
      const auto& op_value = std::any_cast<
          const typename std::decay_t<decltype(trait)>::column_type&>(value);
      result = cpplinq::details::operators::evaluate(column_value, op_value);
    });
    return result;
  }
};
static auto registered_foo_table = []() {
  using trait = table_trait<foo_table, foo_record,
                            cpplinq::details::traits::hash("foo_table")>;
  cpplinq::details::table_registry::instance().add(
      "foo_table", cpplinq::details::traits::any_table{trait{}});
  return true;
}();
using foo_table_table_trait =
    table_trait<foo_table,
                foo_record,
                cpplinq::details::traits::hash("foo_table")>;
template <>
struct index_trait<foo_table_index_1,
                   cpplinq::details::traits::hash("id"),
                   foo_table_table_trait::hash> {
  using index_type = foo_table_index_1;
  using iterator_type = typename index_type::iterator;
  using tuple_type = typename index_type::tuple_type;
  static constexpr auto hash = cpplinq::details::traits::hash("id");
  using table_trait = foo_table_table_trait;
  static const std::string& name() {
    static const auto name_ = std::string{"id"};
    return name_;
  }
  static const std::vector<std::string>& columns() {
    static const auto columns_ = std::vector<std::string>{
        "id",
    };
    return columns_;
  }
  static typename index_type::iterator begin() {
    return index_type::instance().begin();
  }
  static typename index_type::iterator end() {
    return index_type::instance().begin();
  }
  static tuple_type to_tuple(const std::vector<std::string>& v) {
    if (v.size() != std::tuple_size<tuple_type>()) {
      throw cpplinq::details::cpplinq_exception(
          "Input not the same size as tuple type.");
    }
    auto t = tuple_type{};
    auto& columns_ = columns();
    std::get<0>(t) = underlying_column_type<size_t>::from_string(v[0]);
    return t;
  }
  static typename index_type::iterator lower_bound(const tuple_type& t) {
    return index_type::instance().lower_bound(t);
  }
  static typename index_type::iterator upper_bound(const tuple_type& t) {
    return index_type::instance().upper_bound(t);
  }
};
static auto registered_foo_table_index_1 = []() { return true; }();
any_index foo_table_table_trait::index_for(const std::string& name) {
  const static auto indices = std::map<std::string, any_index>{
      {"id",
       {foo_table_table_trait{},
        index_trait<foo_table_index_1, cpplinq::details::traits::hash("id"),
                    foo_table_table_trait::hash>{}}},
  };
  if (indices.empty()) {
    throw cpplinq_exception("Need at least one index defined");
  }
  if (name.empty()) {
    return std::begin(indices)->second;
  }
  auto it = indices.find(name);
  if (it == std::end(indices)) {
    throw cpplinq_exception{"Unknown index"};
  }
  return it->second;
}
}  // namespace cpplinq::details::traits

```

## DECLARE_PROCEDURE

To declare and expose stored procedures, you can refer to the following example.  The only concept that is enforced on the procedure type is that it is a singleton.

```
struct procedures {
  // The only concept enforced on stored procedures is the singleton
  // instance.
  static procedures& instance() {
    static auto instance_ = procedures{};
    return instance_;
  }

  // TODO:  Provide user define conversion operators on cursor
  //        so that this method can just return a string, for example.
  cpplinq::details::cursor foo(const std::string& arg) {
    auto result = cpplinq::details::cursor{};
    result.results.emplace_back();
    result.results.back().emplace_back("foo");
    result.results.back().emplace_back(arg);
    return result;
  }

  cpplinq::details::cursor bar(const std::string& arg1,
                               uint64_t arg2,
                               double arg3) {
    auto result = cpplinq::details::cursor{};
    result.results.emplace_back();
    result.results.back().emplace_back("bar");
    result.results.back().emplace_back(arg1);
    result.results.back().emplace_back(std::to_string(arg2));
    result.results.back().emplace_back(std::to_string(arg3));
    return result;
  }

  cpplinq::details::cursor foobar() {
    auto result = cpplinq::details::cursor{};
    result.results.emplace_back();
    result.results.back().emplace_back("foobar");
    return result;
  }
};

// This makes the procedures known to cpplinq.
DECLARE_PROCEDURE("procedures.foo",
                  procedures,
                  foo,
                  ((arg, std::string, "hello world")));
DECLARE_PROCEDURE("procedures.bar",
                  procedures,
                  bar,
                  ((arg1, std::string))((arg2,
                                         uint64_t))((arg3, double, 3.14)));
DECLARE_PROCEDURE_NO_PARAMETERS("procedures.foobar", procedures, foobar);
```
And you can execute SQL against those procedures similar to the following:
```
// Remember, this is a postgres flavor of SQL.
auto cursor = cpplinq::sql_context::execute("CALL procedures.foo();");
cursor = cpplinq::sql_context::execute("CALL procedures.foo('Hello foo');");
cursor =
    cpplinq::sql_context::execute("CALL procedures.foo(arg => 'Hello foo');");
cursor = cpplinq::sql_context::execute(
    "CALL procedures.bar('Hello bar', 42, 1.1);");
cursor =
    cpplinq::sql_context::execute("CALL procedures.bar('Hello bar', 42);");
cursor = cpplinq::sql_context::execute(
    "CALL procedures.bar('Hello bar', arg2 => 42);");
cursor = cpplinq::sql_context::execute(
    "CALL procedures.bar('Hello bar', arg3 => 1.5, arg2 => 42);");
cursor = cpplinq::sql_context::execute(
    "CALL procedures.bar(arg1 => 'Hello bar', arg3 => 1.5, arg2 => 42);");
cursor = cpplinq::sql_context::execute("CALL procedures.foobar();");
```

The `DECLARE_PROCEDURE` macro will expand to something similar to the following.  This is the generated output of the macro from above.
```
DECLARE_PROCEDURE("procedures.bar",
                  procedures,
                  bar,
                  ((arg1, std::string))((arg2,
                                         uint64_t))((arg3, double, 3.14)));
```                                       
and the code generated from that macro call:
```
namespace cpplinq::details::traits {
template <>
struct parameter_trait<cpplinq::details::traits::hash("arg1"),
                       cpplinq::details::traits::hash("procedures.bar")> {
  constexpr static auto hash = cpplinq::details::traits::hash("arg1");
  constexpr static auto procedure_hash =
      cpplinq::details::traits::hash("procedures.bar");
  constexpr static auto position = size_t{0};
  using type = std::string;
  static type default_value() {
    static const auto default_value_ = type{};
    return default_value_;
  }
  static const type from_string(const std::string& value) {
    if (value.empty()) {
      return default_value();
    }
    return cpplinq::details::traits::underlying_column_type<type>::from_string(
        value);
  }
  static const std::string& procedure_name() {
    static const auto name_ = std::string{"procedures.bar"};
    return name_;
  }
  static const std::string& name() {
    static const auto name_ = std::string{"arg1"};
    return name_;
  }
};
template <>
struct parameter_trait<cpplinq::details::traits::hash("arg2"),
                       cpplinq::details::traits::hash("procedures.bar")> {
  constexpr static auto hash = cpplinq::details::traits::hash("arg2");
  constexpr static auto procedure_hash =
      cpplinq::details::traits::hash("procedures.bar");
  constexpr static auto position = size_t{1};
  using type = uint64_t;
  static type default_value() {
    static const auto default_value_ = type{};
    return default_value_;
  }
  static const type from_string(const std::string& value) {
    if (value.empty()) {
      return default_value();
    }
    return cpplinq::details::traits::underlying_column_type<type>::from_string(
        value);
  }
  static const std::string& procedure_name() {
    static const auto name_ = std::string{"procedures.bar"};
    return name_;
  }
  static const std::string& name() {
    static const auto name_ = std::string{"arg2"};
    return name_;
  }
};
template <>
struct parameter_trait<cpplinq::details::traits::hash("arg3"),
                       cpplinq::details::traits::hash("procedures.bar")> {
  constexpr static auto hash = cpplinq::details::traits::hash("arg3");
  constexpr static auto procedure_hash =
      cpplinq::details::traits::hash("procedures.bar");
  constexpr static auto position = size_t{2};
  using type = double;
  static type default_value() {
    static const auto default_value_ = type{3.14};
    return default_value_;
  }
  static const type from_string(const std::string& value) {
    if (value.empty()) {
      return default_value();
    }
    return cpplinq::details::traits::underlying_column_type<type>::from_string(
        value);
  }
  static const std::string& procedure_name() {
    static const auto name_ = std::string{"procedures.bar"};
    return name_;
  }
  static const std::string& name() {
    static const auto name_ = std::string{"arg3"};
    return name_;
  }
};
template <>
struct procedure_trait<cpplinq::details::traits::hash("procedures.bar")> {
  using procedure_type = procedures;
  static constexpr auto hash = cpplinq::details::traits::hash("procedures.bar");
  static const std::vector<std::string>& parameters() {
    static const auto parameters_ = std::vector<std::string>{
        "arg1",
        "arg2",
        "arg3",
    };
    return parameters_;
  }
  static const std::string& name() {
    static const auto name_ = std::string{"procedures.bar"};
    return name_;
  }
  static cpplinq::details::cursor invoke(const procedure_arguments& args) {
    auto all_args = reconcile<procedure_trait<hash>>(args);
    return procedures::instance().bar(
        parameter_trait<cpplinq::details::traits::hash("arg1"),
                        hash>::from_string(all_args[0].second),
        parameter_trait<cpplinq::details::traits::hash("arg2"),
                        hash>::from_string(all_args[1].second),
        parameter_trait<cpplinq::details::traits::hash("arg3"),
                        hash>::from_string(all_args[2].second));
  }
};
static auto registered_procedures_bar = []() {
  using trait =
      procedure_trait<cpplinq::details::traits::hash("procedures.bar")>;
  cpplinq::details::procedure_registry::instance().add(
      trait::name(), cpplinq::details::traits::any_procedure{trait{}});
  return true;
}();
}  // namespace cpplinq::details::traits
```

## INFORMATION_SCHEMA

Postgres supports an information_schema (https://www.postgresql.org/docs/current/information-schema.html) which allows the end user to dynamically query the database for its structure such as available tables, columns, procedures, etc.  cpplinq supports a subset of that schema to provide the ability to query against those information_schema tables for dynamically discovering the tables and stored procedures exported by the API.

## Aggregates

TBD