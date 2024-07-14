#include <cppling/details/regex.hpp>
#include <cpplinq/details/operators/group.hpp>

using namespace cpplinq::details::operators;

group::group(const std::string &sql) {}

bool group::evaluate() const {
  for (const auto &op : ops) {
    if (!op) {
      return false;
    }
  }
  return true;
}