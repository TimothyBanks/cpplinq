#include <cpplinq/detail/traits/any_table.hpp>

namespace cpplinq::detail::traits {

const std::string& any_table::name() const {
  if (!ptr) {
    throw cpplinq::detail::cpplinq_exception{"Invalid table state"};
  }
  return ptr->name();
}

const std::vector<std::string>& any_table::columns() const {
  if (!ptr) {
    throw cpplinq::detail::cpplinq_exception{"Invalid table state"};
  }
  return ptr->columns();
}
}  // namespace cpplinq::detail::traits