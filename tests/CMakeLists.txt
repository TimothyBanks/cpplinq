add_executable(cpplinq_tests
  tests.cpp
)

target_link_libraries(cpplinq_tests cpplinq)
target_include_directories(cpplinq_tests PUBLIC ../include)