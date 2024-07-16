#pragma once
#include <cstddef>
#include <memory>

namespace cpplinq::details::traits {

struct any_procedure {
 private:
  struct procedure_base {
    virtual const std::string& name() = 0;
  };

  template <typename Procedure_trait>
  struct procedure : public procedure_base {
    using procedure_trait = Procedure_trait;
  };

  std::shared_ptr<procedure_base> ptr;

 public:
  any_procedure() = default;
  any_procedure(const any_procedure&) = default;
  any_procedure(any_procedure&&) = default;
  template <typename Procedure_trait>
  any_procedure(Procedure_trait);

  any_procedure& operator=(const any_procedure&) = default;
  any_procedure& operator=(any_procedure&&) = default;
};

}  // namespace cpplinq::details::traits
