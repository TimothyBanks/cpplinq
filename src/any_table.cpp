#include <cpplinq/detail/cpplinq_exception.hpp>
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

cpplinq::detail::cursor any_table::execute(select_context& context) const {
  if (!ptr) {
    throw cpplinq::detail::cpplinq_exception{"Invalid table state"};
  }
  return ptr->execute(context);
}
}  // namespace cpplinq::detail::traits