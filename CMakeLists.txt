cmake_minimum_required(VERSION 3.22)

project(coroutines LANGUAGES CXX)

add_executable(main main.cpp print.cpp)

set_target_properties(main PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF)

find_package(fmt REQUIRED)
target_link_libraries(main PRIVATE fmt::fmt-header-only)

find_package(GTest CONFIG REQUIRED)
target_link_libraries(main PRIVATE GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)
