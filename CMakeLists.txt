cmake_minimum_required (VERSION 3.8)
project (TRAINING)

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_AR "/usr/bin/llvm-ar")
set(CMAKE_LINKER "/usr/bin/llvm-ld")
set(CMAKE_NM "/usr/bin/llvm-nm")
set(CMAKE_OBJDUMP "/usr/bin/llvm-objdump")
set(CMAKE_RANLIB "/usr/bin/llvm-ranlib")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost 1.82.0)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

add_library(cpplinq STATIC  
    src/call_context.cpp 
    src/column.cpp 
    src/count.cpp
    src/delete_context.cpp 
    src/insert_context.cpp 
    src/procedure.cpp 
    src/select_context.cpp 
    src/sum.cpp 
    src/table.cpp 
    src/update_context.cpp 
    src/where_tree.cpp)
target_include_directories (cpplinq PUBLIC ./include)