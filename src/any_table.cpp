#include <cpplinq/details/cpplinq_exception.hpp>
#include <cpplinq/details/traits/any_table.hpp>

namespace cpplinq::details::traits {

const std::string& any_table::name() const {
  if (!ptr) {
    throw cpplinq::details::cpplinq_exception{"Invalid table state"};
  }
  return ptr->name();
}

const std::vector<std::string>& any_table::columns() const {
  if (!ptr) {
    throw cpplinq::details::cpplinq_exception{"Invalid table state"};
  }
  return ptr->columns();
}

cpplinq::details::cursor any_table::execute(
    const select_context& context) const {
  if (!ptr) {
    throw cpplinq::details::cpplinq_exception{"Invalid table state"};
  }
  return ptr->execute(context);
}
}  // namespace cpplinq::details::traits