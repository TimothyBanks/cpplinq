#pragma once
#include <cpplinq/detail/cursor.hpp>
#include <cpplinq/detail/traits/procedure_trait.hpp>
#include <cstddef>
#include <memory>

namespace cpplinq::detail::traits {

struct any_procedure {
 private:
  struct procedure_base {
    virtual const std::string& name() = 0;
    virtual cpplinq::detail::cursor invoke(const procedure_arguments& args) = 0;
  };

  template <typename Procedure_trait>
  struct procedure : public procedure_base {
    using procedure_trait = Procedure_trait;

    virtual const std::string& name() override {
      return procedure_trait::name();
    }
    virtual cpplinq::detail::cursor invoke(
        const procedure_arguments& args) override {
      return procedure_trait::invoke(args);
    }
  };

  std::shared_ptr<procedure_base> ptr;

 public:
  any_procedure() = default;
  any_procedure(const any_procedure&) = default;
  any_procedure(any_procedure&&) = default;
  template <typename Procedure_trait>
  any_procedure(Procedure_trait)
      : ptr{std::make_unique<procedure<Procedure_trait>>()} {}

  any_procedure& operator=(const any_procedure&) = default;
  any_procedure& operator=(any_procedure&&) = default;

  const std::string& name() {
    if (!ptr) {
      throw cpplinq_exception{"Invalid state"};
    }
    return ptr->name();
  }
  cpplinq::detail::cursor invoke(const procedure_arguments& args) {
    if (!ptr) {
      throw cpplinq_exception{"Invalid state"};
    }
    return ptr->invoke(args);
  }
};

}  // namespace cpplinq::detail::traits
