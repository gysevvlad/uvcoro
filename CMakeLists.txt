cmake_minimum_required(VERSION 3.22)

project(uvcore LANGUAGES CXX)

add_library(uvcoro STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/operation/detail/read.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/operation/delay.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/operation/read.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/operation/read_or_die.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/promise.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tcp/acceptor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/unit.cpp)

target_include_directories(uvcoro PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(uvcoro PROPERTIES
            CXX_STANDARD 20
            CXX_EXTENSIONS ON)

find_package(fmt REQUIRED)
target_link_libraries(uvcoro PRIVATE fmt::fmt-header-only)

#find_package(GTest CONFIG REQUIRED)
#target_link_libraries(main PRIVATE GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)

find_package(unofficial-libuv CONFIG REQUIRED)
target_link_libraries(uvcoro PUBLIC unofficial::libuv::libuv)

#find_package(nlohmann_json CONFIG REQUIRED)
#target_link_libraries(main PRIVATE nlohmann_json nlohmann_json::nlohmann_json)

#find_package(Boost REQUIRED COMPONENTS program_options)
#target_link_libraries(main PRIVATE Boost::program_options)

add_executable(echo_server
    ${CMAKE_CURRENT_SOURCE_DIR}/example/echo_server.cpp)

set_target_properties(echo_server PROPERTIES
            CXX_STANDARD 20
            CXX_EXTENSIONS ON)

target_link_libraries(echo_server PRIVATE uvcoro)
