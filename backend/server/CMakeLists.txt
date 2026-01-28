cmake_minimum_required(VERSION 3.12)
project(server_project)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(nlohmann_json REQUIRED)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
pkg_check_modules(PISTACHE REQUIRED IMPORTED_TARGET libpistache)
include_directories(${Boost_INCLUDE_DIRS})
add_executable(server src/main.cpp headers/ServerHandler.h)

target_compile_options(server PRIVATE 
    -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor 
    -Wold-style-cast -Woverloaded-virtual -Werror
)

target_include_directories(server PRIVATE 
    ${PISTACHE_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src 
    ${CMAKE_SOURCE_DIR}/headers
)

target_link_libraries(server PRIVATE 
    nlohmann_json::nlohmann_json
    Threads::Threads
    PkgConfig::PISTACHE
    Boost::system
)