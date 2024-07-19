#include <cpplinq/details/cpplinq_exception.hpp>
#include <cpplinq/details/traits/any_index.hpp>

namespace cpplinq::details::traits {

const std::string& any_index::name() const {
  if (!ptr) {
    throw cpplinq::details::cpplinq_exception{"Invalid table state"};
  }
  return ptr->name();
}

const std::vector<std::string>& any_index::columns() const {
  if (!ptr) {
    throw cpplinq::details::cpplinq_exception{"Invalid table state"};
  }
  return ptr->columns();
}

cpplinq::details::cursor any_index::execute(
    select_context& context) const {
  if (!ptr) {
    throw cpplinq::details::cpplinq_exception{"Invalid table state"};
  }
  return ptr->execute(context);
}
}  // namespace cpplinq::details::traits