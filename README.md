# cpplinq
Library for overlaying an SQL syntax onto cpp data structures.  This will allow for executing various SQL statements against your data structures such as SELECT, INSERT, CALL.  This library will follow a postgres flavor of SQL.

The concept of this library came about as a possible way to white-box blockchain data.  While blockchains are known for transaparency, getting to meaningful data can be hard to do.  This is also coupled with people might not be familiar with toolchains, APIs, languages, etc that are used with the blockchain that is holding the data.  This is why SQL was choosen as the facade, because most people are comfortable with that toolset and language.  This also allows for future enhancements such as extending the library to mimic a DBMS and conforming to ODBC/JDBC standards, which would allow the blockchain to be integrated into your system as another SQL based data source.

For now, it is much easier to show an example of the library to demonstrate how to use and hopefully give an understanding of how it could be used.  The are a few major concepts that this library is employing:
1.  Duck typing.  There is an implicit interface that this library requires for integrators, but this interface can be exposed on your existing types with minimal changes.  One of the great features of CPP templates is duck typing, "if it walks like a duck and talks like a duck, it's a duck", or in our context, if the type has a method named the same as the method required by the API and matches the function signature, then it must be a type compatible with us.
2.  Type traits.  This is a solid design pattern globally accepted by the CPP community that allows encapsulation of most of this information needed by cpplinq.  This nice thing about this is that the information is also available at compile time.
3.  Macros.  Most of this library is built on the concept of code generation and that is exactly what Macros give us.

Additional features that could be looked into is the use of an LLVM and clang compiler plugin that could generate this code at compile time using simple annotations on your types.

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
    using table_index_type = std::tuple<T...>;
    using table_type       = Table;
    using backing_type     = std::map<table_index_type, size_t>;

    backing_type index_;
    const table_type* table_;

    table_index() = default;
    table_index(const table_index&) = default;
    table_index(table_index&&) = default;

    table_index(const table_type& table) : table_{&table} {}

    table_index& operator=(const table_index&) = default;
    table_index& operator=(table_index&&) = default;

    void push(table_index_type key, size_t position) {
        index_.emplace(std::move(key), position);
    }

    void pop(const table_index_type& key) {
        index_.remove(key);
    }

    struct iterator {
        typename backing_type::iterator begin_;
        typename backing_type::iterator end_;
        mutable typename backing_type::iterator it_;
        table_type* table_;

        iterator() = default;
        iterator(const iterator&) = default;
        iterator(iterator&&) = default;

        iterator(table_type& table, typename backing_type::iterator begin, typename backing_type::iterator end, typename backing_type::iterator it)
            : table_{&table}, begin_{std::move(begin)}, end_{std::move(end)}, it_{std::move(it)} {}

        iterator& operator=(const iterator&) = default;
        iterator& operator=(iterator&&) = default;

        typename table_type::record_type& operator*() { 
            auto position = it_.second;
            if (position >= table_->data().size()) {
                throw "iterator out of bounds";
            }
            return *(table_->data()[position]); 
        }

        const typename table_type::record_type& operator*() const  { 
            auto position = it_.second;
            if (position >= table_->data().size()) {
                throw "iterator out of bounds";
            }
            return *(table_->data()[position]); 
        }

        iterator& operator++() { ++it_; return *this; }
        iterator& operator--() { --it_; return *this; }
        const iterator& operator++() const { ++it_; return *this; }
        const iterator& operator--() const { --it_; return *this; }

        bool operator==(const iterator& other) const { return it_ == other.it_; }
        bool operator!=(const iterator& other) const { return it_ != other.it_; }
    };

    iterator begin() {
        return iterator{*table_, std::begin(index_), std::end(index_), std::begin(index_)};
    }

    iterator end() {
        return iterator{*table_, std::begin(index_), std::end(index_), std::end(index_)};
    }

    iterator lower_bound(const table_index_type& key) {
        return iterator{*table_, std::begin(index_), std::end(index_), index_.lower_bound(key)};
    }

    iterator upper_bound(const table_index_type& key) {
        return iterator{*table_, std::begin(index_), std::end(index_), index_.upper_bound(key)};
    }

    const iterator begin() const {
        return iterator{*table_, std::begin(index_), std::end(index_), std::begin(index_)};
    }

    const iterator end() const{
        return iterator{*table_, std::begin(index_), std::end(index_), std::end(index_)};
    }

    const iterator lower_bound(const table_index_type& key) const {
        return iterator{*table_, std::begin(index_), std::end(index_), index_.lower_bound(key)};
    }

    const iterator upper_bound(const table_index_type& key) const {
        return iterator{*table_, std::begin(index_), std::end(index_), index_.upper_bound(key)};
    }
};

// cpplinq utilizes a concept called duck typing.
// For a table, the following concepts are expected:
// 1.  It is a singleton and exposes a static method called instance.
// 2.  It has an instance method, called primary_index, to retrieve the primary index to iterate on.
// 3.  The "index" type returned must conform to the range concept.
// 4.  As the development of this library progresses, there may be additional concepts centered around secondary indices.
struct foo_table {
    friend struct table_index<foo_table, uint64_t>;

    using record_type = foo_record;
    using backing_store = std::vector<record_type>;

    table_index<foo_table, uint64_t> primary_index_;
    backing_store records_;

    foo_table() : primary_index_{*this} {}
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
    }

    void pop(size_t index) {
        if (index >= records_.size()) {
            return;
        }
        records_.erase(std::begin(records_) + index);
    }

    backing_store& data() { return records_; }
    const backing_store& data() const { return records_; }

    const table_index<foo_table, uint64_t>& primary_index() const {
        return primary_index_;
    }

    table_index<foo_table, uint64_t>& primary_index() {
        return primary_index_;
    }
};

// This makes the table known to cpplinq.
DECLARE_TABLE("foo_table", foo_table, foo_record,
  ((id, uint64_t))((foo, std::string))((bar, float))((foobar, std::vector<std::string>)), ());

...

// Populate your table.
...

// Then as a simple example of how this could be used
auto context = cpplinq::sql_context{};
auto cursor = context.execute("SELECT foo, bar, foobar FROM foo_table WHERE id > 100 AND id < 200;");

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
  using column_type = uint64_t;
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
  static const std::string& name() { return "foo_table"; }
  static const std::vector<std::string>& columns() {
    static const auto columns_ = std::vector<std::string>{
        "id",
        "foo",
        "bar",
        "foobar",
    };
    return columns_;
  }
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
      result =
          underlying_column_type_t<declytpe(trait)::column_type>::from_string(
              value);
    });
    return result;
  }
  static std::string to_string(const std::string& column_name,
                               const record_type& record) {
    auto result = std::string{};
    invoke(column_name, [&](const auto& trait) {
      const auto& value = trait.value(record);
      result =
          underlying_column_type_t<decltype(trait)::column_type>::to_string(
              value);
    });
    return result;
  }
  static bool evaluate(
      const record_type& record,
      const cpplinq::details::operators::expression_tree& expression) {
    return evaluate<type>(record, expression);
  }
  static std::unordered_set<cpplinq::details::operators::comparison_result>
  evaluate(const std::string& column_name,
           const record_type& record,
           const std::any& value) {
    auto result =
        std::unordered_set<cpplinq::details::operators::comparison_result>{};
    invoke(column_name, [&](const auto& trait) {
      const auto& column_value = trait.value(record);
      const auto& op_value =
          std::any_cast<decltype(trait)::column_type&>(value);
      result = evaluate(column_value, op_value);
    });
    return result;
  }
};
static auto registered_foo_table = []() {
  using trait = table_trait<foo_table, foo_record,
                            cpplinq::details::traits::hash("foo_table")>;
  cpplinq::details::table_registry::instance().add(
      "foo_table", cpplinq::details::traits::any_table<trait>{});
  return true;
}();
}  // namespace cpplinq::details::traits
```