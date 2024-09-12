#pragma once
#include <string>

namespace cpplinq::detail {

struct column {
  std::string name;
  std::string alias;
  std::string table;
  std::string aggregate;
  std::string type;
  std::string value;
};

}  // namespace cpplinq::detail