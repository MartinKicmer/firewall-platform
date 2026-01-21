cmake_minimum_required(VERSION 3.12)
project(server_project)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(nlohmann_json REQUIRED)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(PISTACHE REQUIRED IMPORTED_TARGET libpistache)

add_executable(server src/main.cpp)

target_compile_options(server PRIVATE 
    -Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor 
    -Wold-style-cast -Wunused -Woverloaded-virtual -Werror
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
)