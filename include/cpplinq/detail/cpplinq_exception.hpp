#pragma once
#include <exception>
#include <string>

namespace cpplinq::detail {

struct cpplinq_exception : public std::exception {
  std::string what_string;

  cpplinq_exception() = default;
  cpplinq_exception(const cpplinq_exception&) = default;
  cpplinq_exception(cpplinq_exception&&) = default;
  cpplinq_exception(std::string what_) : what_string{std::move(what_)} {}

  cpplinq_exception& operator=(const cpplinq_exception&) = default;
  cpplinq_exception& operator=(cpplinq_exception&&) = default;

  virtual const char* what() const noexcept override { return what_string.c_str(); }
};

}  // namespace cpplinq::detail