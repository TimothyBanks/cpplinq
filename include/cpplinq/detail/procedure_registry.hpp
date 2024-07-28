#pragma once
#include <cpplinq/detail/traits/any_procedure.hpp>
#include <unordered_map>

namespace cpplinq::detail {

struct procedure_registry {
 private:
  std::unordered_map<std::string, traits::any_procedure> procedures;

 public:
  static procedure_registry& instance();
  void add(std::string procedure_name, traits::any_procedure procedure);
  void remove(const std::string& procedure_name);
  traits::any_procedure& find(const std::string& procedure_name);
  const traits::any_procedure& find(const std::string& procedure_name) const;
};

}  // namespace cpplinq::detail