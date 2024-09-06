#pragma once
#include <cpplinq/detail/cpplinq_exception.hpp>
#include <cstdlib>
#include <map>
#include <tuple>

namespace cpplinq::detail::information_schema {

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

  void pop(const tuple_type& key) { 
    auto it = lower_bound(key);
    auto position = it.it_->second;

    // This is a very naive approach
    for (auto it = std::begin(index_); it != std::end(index_); ++it) {
      if (it->second <= position) {
        continue;
      }
      --it->second;
    }

    index_.erase(key); 
  }

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
        throw cpplinq::detail::cpplinq_exception{"iterator out of bounds"};
      }
      return table_->data()[position];
    }

    const typename table_type::record_type& operator*() const {
      auto position = it_->second;
      if (position >= table_->data().size()) {
        throw cpplinq::detail::cpplinq_exception{"iterator out of bounds"};
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

  iterator erase(iterator& it) {
    // Get the next entry
    if (it == std::end(*this)) {
      return std::end(*this);
    }

    ++it;
    auto key = tuple_type{};
    if (it != std::end(*this)) {
      key = it.it_->first;
    }
    --it;

    table_->pop(it.it_->second);

    if (key == tuple_type{}) {
      return end();
    }
    return lower_bound(key);
  }
};

}  // namespace cpplinq::detail::information_schema