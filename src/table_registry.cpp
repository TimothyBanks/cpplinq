#include <cpplinq/details/cpplinq_exception.hpp>
#include <cpplinq/details/table_registry.hpp>

using namespace cpplinq::details;

table_registry& table_registry::instance() {
  static auto instance = table_registry{};
  return instance;
}

void table_registry::add(std::string table_name, traits::any_table table) {
  tables.emplace(std::move(table_name), std::move(table));
}

void table_registry::remove(const std::string& table_name) {
  tables.erase(table_name);
}

traits::any_table& table_registry::find(const std::string& table_name) {
  auto it = tables.find(table_name);
  if (it == std::end(tables)) {
    throw cpplinq::details::cpplinq_exception{"Table not found"};
  }
  return it->second;
}

const traits::any_table& table_registry::find(
    const std::string& table_name) const {
  auto it = tables.find(table_name);
  if (it == std::end(tables)) {
    throw cpplinq::details::cpplinq_exception{"Table not found"};
  }
  return it->second;
}
