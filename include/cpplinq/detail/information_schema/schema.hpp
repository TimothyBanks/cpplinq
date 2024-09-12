#pragma once
#include <cpplinq/detail/information_schema/table_index.hpp>
#include <cstdlib>
#include <string>
#include <vector>

namespace cpplinq::detail::information_schema {

// cpplinq places no concepts on the table structure.
struct column {
  std::string table_catalog;
  std::string table_schema;
  std::string table_name;
  std::string column_name;
  size_t ordinal_position{0};
  std::string column_default;
  bool is_nullable{false};
  std::string data_type;
  bool is_updatable{true};
};

struct parameter {
  std::string specific_catalog;
  std::string specific_schema;
  std::string specific_name;
  size_t ordinal_position{0};
  std::string parameter_name;
  std::string data_type;
  std::string parameter_default;
};

struct routine {
  std::string specific_catalog;
  std::string specific_schema;
  std::string specific_name;
  std::string routine_name;
  std::string data_type;
};

struct table {
  std::string table_catalog;
  std::string table_schema;
  std::string table_name;
};

// cpplinq utilizes a concept called duck typing.
// For a table, the following concepts are expected:
// 1.  It is a singleton and exposes a static method called instance.
// 2.  It has an instance method, called primary_index, to retrieve the primary
// index to iterate on.
// 3.  The "index" type returned must conform to the range concept.
// 4.  As the development of this library progresses, there may be additional
// concepts centered around secondary indices.
struct columns {
  using index =
      table_index<columns, std::string, std::string, std::string, std::string>;
  friend struct table_index<columns,
                            std::string,
                            std::string,
                            std::string,
                            std::string>;

  using record_type = column;
  using backing_store = std::vector<record_type>;

  backing_store records_;

  columns();
  columns(const columns&) = default;
  columns(columns&&) = default;

  columns& operator=(const columns&) = default;
  columns& operator=(columns&&) = default;

  static columns& instance();
  void push_back(record_type record);
  void erase(size_t index);
  backing_store& data();
  const backing_store& data() const;
};

struct parameters {
  using index = table_index<parameters,
                            std::string,
                            std::string,
                            std::string,
                            std::string>;
  friend struct table_index<parameters,
                            std::string,
                            std::string,
                            std::string,
                            std::string>;

  using record_type = parameter;
  using backing_store = std::vector<record_type>;

  backing_store records_;

  parameters();
  parameters(const parameters&) = default;
  parameters(parameters&&) = default;

  parameters& operator=(const parameters&) = default;
  parameters& operator=(parameters&&) = default;

  static parameters& instance();
  void push_back(record_type record);
  void erase(size_t index);
  backing_store& data();
  const backing_store& data() const;
};

struct routines {
  using index =
      table_index<routines, std::string, std::string, std::string, std::string>;
  friend struct table_index<routines,
                            std::string,
                            std::string,
                            std::string,
                            std::string>;

  using record_type = routine;
  using backing_store = std::vector<record_type>;

  backing_store records_;

  routines();
  routines(const routines&) = default;
  routines(routines&&) = default;

  routines& operator=(const routines&) = default;
  routines& operator=(routines&&) = default;

  static routines& instance();
  void push_back(record_type record);
  void erase(size_t index);
  backing_store& data();
  const backing_store& data() const;
};

struct tables {
  using index = table_index<tables, std::string, std::string, std::string>;
  friend struct table_index<tables, std::string, std::string, std::string>;

  using record_type = table;
  using backing_store = std::vector<record_type>;

  backing_store records_;

  tables();
  tables(const tables&) = default;
  tables(tables&&) = default;

  tables& operator=(const tables&) = default;
  tables& operator=(tables&&) = default;

  static tables& instance();
  void push_back(record_type record);
  void erase(size_t index);
  backing_store& data();
  const backing_store& data() const;
};

}  // namespace cpplinq::detail::information_schema
