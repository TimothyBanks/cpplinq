#define BOOST_TEST_MODULE select_context_test

#include <boost/test/included/unit_test.hpp>
#include <cpplinq/cpplinq.hpp>
#include <map>
#include <tuple>
#include <vector>

// cpplinq places no concepts on the table structure.
struct foo_record {
  size_t id;
  std::string foo;
  float bar;
  std::vector<std::string> foobar;
};

struct foobar_record {
  size_t identifier;
  std::string data;
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
  using table_type = Table;
  using backing_type = std::map<table_index_type, size_t>;

  backing_type index_;
  table_type* table_;

  table_index() = default;
  table_index(const table_index&) = default;
  table_index(table_index&&) = default;

  table_index(table_type& table) : table_{&table} {}

  table_index& operator=(const table_index&) = default;
  table_index& operator=(table_index&&) = default;

  void push(table_index_type key, size_t position) {
    index_.emplace(std::move(key), position);
  }

  void pop(const table_index_type& key) { index_.erase(key); }

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

  iterator lower_bound(const table_index_type& key) {
    return iterator{*table_, std::begin(index_), std::end(index_),
                    index_.lower_bound(key)};
  }

  iterator upper_bound(const table_index_type& key) {
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

  const iterator lower_bound(const table_index_type& key) const {
    return iterator{*table_, std::begin(index_), std::end(index_),
                    index_.lower_bound(key)};
  }

  const iterator upper_bound(const table_index_type& key) const {
    return iterator{*table_, std::begin(index_), std::end(index_),
                    index_.upper_bound(key)};
  }
};

// cpplinq utilizes a concept called duck typing.
// For a table, the following concepts are expected:
// 1.  It is a singleton and exposes a static method called instance.
// 2.  It has an instance method, called primary_index, to retrieve the primary
// index to iterate on.
// 3.  The "index" type returned must conform to the range concept.
// 4.  As the development of this library progresses, there may be additional
// concepts centered around secondary indices.
struct foo_table {
  friend class table_index<foo_table, size_t>;

  using record_type = foo_record;
  using backing_store = std::vector<record_type>;
  using primary_index_type = table_index<foo_table, size_t>;

  primary_index_type primary_index_;
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
    primary_index_.push(std::make_tuple(record.id), records_.size() - 1);
  }

  void pop(size_t index) {
    if (index >= records_.size()) {
      return;
    }
    primary_index_.pop(std::make_tuple(records_[index].id));
    records_.erase(std::begin(records_) + index);
  }

  backing_store& data() { return records_; }
  const backing_store& data() const { return records_; }

  const primary_index_type& primary_index() const {
    return primary_index_;
  }

  primary_index_type& primary_index() { return primary_index_; }
};

struct foobar_table {
  friend class table_index<foobar_table, size_t>;

  using record_type = foobar_record;
  using backing_store = std::vector<record_type>;
  using primary_index_type = table_index<foobar_table, size_t>;

  primary_index_type primary_index_;
  backing_store records_;

  foobar_table() : primary_index_{*this} {}
  foobar_table(const foobar_table&) = default;
  foobar_table(foobar_table&&) = default;

  foobar_table& operator=(const foobar_table&) = default;
  foobar_table& operator=(foobar_table&&) = default;

  static foobar_table& instance() {
    static auto instance_ = foobar_table{};
    return instance_;
  }

  void push(record_type record) {
    records_.emplace_back();
    records_.back() = std::move(record);
    primary_index_.push(std::make_tuple(record.identifier), records_.size() - 1);
  }

  void pop(size_t index) {
    if (index >= records_.size()) {
      return;
    }
    primary_index_.pop(std::make_tuple(records_[index].identifier));
    records_.erase(std::begin(records_) + index);
  }

  backing_store& data() { return records_; }
  const backing_store& data() const { return records_; }

  const primary_index_type& primary_index() const {
    return primary_index_;
  }

  primary_index_type& primary_index() {
    return primary_index_;
  }
};

// This makes the table known to cpplinq.
DECLARE_TABLE("foo_table",
              foo_table,
              foo_record,
              ((id, uint64_t))((foo, std::string))((bar, float))(
                  (foobar, std::vector<std::string>)),
              ());
DECLARE_TABLE("foobar_table",
              foobar_table,
              foobar_record,
              ((identifier, uint64_t))((data, std::string)),
              ());

BOOST_AUTO_TEST_CASE(select_context) {
  auto& ft = foo_table::instance();

  for (auto i = size_t{0}; i < 100; ++i) {
    ft.push({
        .id = i,
        .foo = std::to_string(i),
        .bar = static_cast<float>(i),
        .foobar = {std::to_string(i), std::to_string(i), std::to_string(i)}
    });
  }

  auto& fbt = foobar_table::instance();

  for (auto i = size_t{1000}; i < 1100; ++i) {
    fbt.push({
        .identifier = i,
        .data = std::to_string(i) + std::to_string(i) + std::to_string(i)
    });
  }

  auto cursor = cpplinq::sql_context::execute("SELECT id, foo, bar, foobar FROM foo_table WHERE (id = 55 OR id = 60) OR (id > 70 AND id < 75) OR foo = '42'");
  cursor = cpplinq::sql_context::execute("SELECT * FROM foo_table");
  cursor = cpplinq::sql_context::execute("SELECT * FROM foobar_table");
  cursor = cpplinq::sql_context::execute("SELECT indentifier, data FROM foobar_table");
}