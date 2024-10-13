cmake_minimum_required(VERSION 3.10)

project(Onol VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB HEADERS "${PROJECT_SOURCE_DIR}/src/*.hpp" "${PROJECT_SOURCE_DIR}/include/*.hpp")

set(PCH_HEADER "${PROJECT_SOURCE_DIR}/include/pch.hpp")

add_executable(Onol ${SOURCES} ${HEADERS})

target_precompile_headers(Onol PRIVATE ${PCH_HEADER})

target_include_directories(Onol PUBLIC "${PROJECT_SOURCE_DIR}/include")

if(MSVC)
    target_compile_options(Onol PRIVATE /W4)
else()
    target_compile_options(Onol PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

set(CMAKE_BUILD_TYPE Debug)

find_package(Catch2 3 REQUIRED)

add_executable(tests tests/test_allocator.cpp src/allocator.cpp tests/test_bytecode.cpp src/bytecode.cpp)
target_include_directories(tests PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

target_precompile_headers(tests PRIVATE ${PCH_HEADER})

include(CTest)
include(Catch)
catch_discover_tests(tests)
