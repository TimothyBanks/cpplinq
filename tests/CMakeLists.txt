add_executable(cpplinq_tests
  sql_context_tests.cpp
)

target_link_libraries(cpplinq_tests cpplinq)
target_include_directories(cpplinq_tests PUBLIC ../include)