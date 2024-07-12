#pragma once
#include <string>

namespace cpplinq::details {

struct column {
  std::string name;
  std::string alias;
  std::string table;
  std::string aggregate;
  std::string type;
};

} // namespace cpplinq::details