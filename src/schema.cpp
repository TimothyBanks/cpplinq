#include <cpplinq/details/information_schema/schema.hpp>
#include <cpplinq/details/table_registry.hpp>
#include <cpplinq/details/traits/table_trait.hpp>

using columns_index = cpplinq::details::information_schema::columns::index;
using information_schema_columns = cpplinq::details::information_schema::columns;
using information_schema_column = cpplinq::details::information_schema::column;
DECLARE_TABLE("information_schema.columns",
              information_schema_columns,
              information_schema_column,
              ((table_catalog, std::string))((table_schema, std::string))(
                  (table_name, std::string))((column_name, std::string))(
                  (ordinal_position, size_t))((column_default, std::string))(
                  (is_nullable, bool))((data_type, std::string))((is_updatable,
                                                                  bool)),
              (("id",
                columns_index,
                ((table_catalog, std::string))((table_schema, std::string))(
                    (table_name, std::string))((column_name, std::string)))));

using parameters_index =
    cpplinq::details::information_schema::parameters::index;
using information_schema_parameters = cpplinq::details::information_schema::parameters;
using information_schema_parameter = cpplinq::details::information_schema::parameter;
DECLARE_TABLE(
    "information_schema.parameters",
    information_schema_parameters,
    information_schema_parameter,
    ((specific_catalog, std::string))((specific_schema, std::string))(
        (specific_name, std::string))((ordinal_position,
                                       size_t))((parameter_name, std::string))(
        (data_type, std::string))((parameter_default, std::string)),
    (("id",
      parameters_index,
      ((specific_catalog, std::string))((specific_schema, std::string))(
          (specific_name, std::string))((parameter_name, std::string)))));

using routines_index = cpplinq::details::information_schema::routines::index;
using information_schema_routines = cpplinq::details::information_schema::routines;
using information_schema_routine = cpplinq::details::information_schema::routine;
DECLARE_TABLE(
    "information_schema.routines",
    information_schema_routines,
    information_schema_routine,
    ((specific_catalog, std::string))((specific_schema, std::string))(
        (specific_name, std::string))((routine_name,
                                       std::string))((data_type, std::string)),
    (("id",
      routines_index,
      ((specific_catalog, std::string))((specific_schema, std::string))(
          (specific_name, std::string))((routine_name, std::string)))));

using tables_index = cpplinq::details::information_schema::tables::index;
using information_schema_tables = cpplinq::details::information_schema::tables;
using information_schema_table = cpplinq::details::information_schema::table;
DECLARE_TABLE("information_schema.tables",
              information_schema_tables,
              information_schema_table,
              ((table_catalog, std::string))((table_schema, std::string))(
                  (table_name, std::string)),
              (("id",
                tables_index,
                ((table_catalog, std::string))((table_schema, std::string))(
                    (table_name, std::string)))));

using namespace cpplinq::details::information_schema;

columns::columns() {
  index::instance().table(*this);
}

columns& columns::instance() {
  static auto instance_ = columns{};
  return instance_;
}

void columns::push(record_type record) {
  records_.emplace_back();
  index::instance().push(
      std::make_tuple(record.table_catalog, record.table_schema,
                      record.table_name, record.column_name),
      records_.size() - 1);
  records_.back() = std::move(record);
}

void columns::pop(size_t index) {
  if (index >= records_.size()) {
    return;
  }
  auto& record = records_[index];
  index::instance().pop(std::make_tuple(record.table_catalog,
                                        record.table_schema, record.table_name,
                                        record.column_name));
  records_.erase(std::begin(records_) + index);
}

columns::backing_store& columns::data() {
  return records_;
}
const columns::backing_store& columns::data() const {
  return records_;
}

parameters::parameters() {
  index::instance().table(*this);
}

parameters& parameters::instance() {
  static auto instance_ = parameters{};
  return instance_;
}

void parameters::push(record_type record) {
  records_.emplace_back();
  index::instance().push(
      std::make_tuple(record.specific_catalog, record.specific_schema,
                      record.specific_name, record.parameter_name),
      records_.size() - 1);
  records_.back() = std::move(record);
}

void parameters::pop(size_t index) {
  if (index >= records_.size()) {
    return;
  }
  auto& record = records_[index];
  index::instance().pop(
      std::make_tuple(record.specific_catalog, record.specific_schema,
                      record.specific_name, record.parameter_name));
  records_.erase(std::begin(records_) + index);
}

parameters::backing_store& parameters::data() {
  return records_;
}
const parameters::backing_store& parameters::data() const {
  return records_;
}

routines::routines() {
  index::instance().table(*this);
}

routines& routines::instance() {
  static auto instance_ = routines{};
  return instance_;
}

void routines::push(record_type record) {
  records_.emplace_back();
  index::instance().push(
      std::make_tuple(record.specific_catalog, record.specific_schema,
                      record.specific_name, record.routine_name),
      records_.size() - 1);
  records_.back() = std::move(record);
}

void routines::pop(size_t index) {
  if (index >= records_.size()) {
    return;
  }
  auto& record = records_[index];
  index::instance().pop(
      std::make_tuple(record.specific_catalog, record.specific_schema,
                      record.specific_name, record.routine_name));
  records_.erase(std::begin(records_) + index);
}

routines::backing_store& routines::data() {
  return records_;
}
const routines::backing_store& routines::data() const {
  return records_;
}

tables::tables() {
  index::instance().table(*this);
}

tables& tables::instance() {
  static auto instance_ = tables{};
  return instance_;
}

void tables::push(record_type record) {
  records_.emplace_back();
  index::instance().push(
      std::make_tuple(record.table_catalog, record.table_schema,
                      record.table_name),
      records_.size() - 1);
  records_.back() = std::move(record);
}

void tables::pop(size_t index) {
  if (index >= records_.size()) {
    return;
  }
  auto& record = records_[index];
  index::instance().pop(std::make_tuple(
      record.table_catalog, record.table_schema, record.table_name));
  records_.erase(std::begin(records_) + index);
}

tables::backing_store& tables::data() {
  return records_;
}
const tables::backing_store& tables::data() const {
  return records_;
}
