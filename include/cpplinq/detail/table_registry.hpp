#pragma once
#include <cpplinq/detail/traits/any_table.hpp>
#include <unordered_map>

namespace cpplinq::detail {

struct table_registry {
 private:
  std::unordered_map<std::string, traits::any_table> tables;

 public:
  static table_registry& instance();
  void add(std::string table_name, traits::any_table table);
  void remove(const std::string& table_name);
  traits::any_table& find(const std::string& table_name);
  const traits::any_table& find(const std::string& table_name) const;
};

}  // namespace cpplinq::detail