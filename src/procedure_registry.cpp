#include <cpplinq/detail/cpplinq_exception.hpp>
#include <cpplinq/detail/procedure_registry.hpp>

using namespace cpplinq::detail;

procedure_registry& procedure_registry::instance() {
  static auto instance = procedure_registry{};
  return instance;
}

void procedure_registry::add(std::string procedure_name,
                             traits::any_procedure procedure) {
  procedures.emplace(std::move(procedure_name), std::move(procedure));
}

void procedure_registry::remove(const std::string& procedure_name) {
  procedures.erase(procedure_name);
}

traits::any_procedure& procedure_registry::find(
    const std::string& procedure_name) {
  auto it = procedures.find(procedure_name);
  if (it == std::end(procedures)) {
    throw cpplinq::detail::cpplinq_exception{"Procedure not found"};
  }
  return it->second;
}

const traits::any_procedure& procedure_registry::find(
    const std::string& procedure_name) const {
  auto it = procedures.find(procedure_name);
  if (it == std::end(procedures)) {
    throw cpplinq::detail::cpplinq_exception{"Procedure not found"};
  }
  return it->second;
}
