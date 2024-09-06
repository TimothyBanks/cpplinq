#include <cpplinq/detail/cpplinq_exception.hpp>
#include <cpplinq/detail/traits/any_index.hpp>

namespace cpplinq::detail::traits {

const std::string& any_index::name() const {
  if (!ptr) {
    throw cpplinq::detail::cpplinq_exception{"Invalid table state"};
  }
  return ptr->name();
}

const std::vector<std::string>& any_index::columns() const {
  if (!ptr) {
    throw cpplinq::detail::cpplinq_exception{"Invalid table state"};
  }
  return ptr->columns();
}
}  // namespace cpplinq::detail::traits