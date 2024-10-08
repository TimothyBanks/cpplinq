cmake_minimum_required (VERSION 3.8)
project (cpplinq)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost 1.85.0)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

add_library(cpplinq STATIC
    src/aggregate.cpp
    src/any_index.cpp
    src/any_procedure.cpp  
    src/any_table.cpp
    src/call_context.cpp
    src/delete_context.cpp 
    src/hash.cpp
    src/insert_context.cpp 
    src/operators_make.cpp
    src/procedure_registry.cpp
    src/range_info.cpp
    src/regex.cpp
    src/select_context.cpp 
    src/schema.cpp
    src/sql_context.cpp
    src/string.cpp
    src/table_registry.cpp
    src/unsupported.cpp
    src/update_context.cpp)

target_include_directories(cpplinq PUBLIC ./include)

# if(CPPLINQ_BUILD_TESTS)
add_subdirectory(tests)
# endif()