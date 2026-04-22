cmake_minimum_required(VERSION 3.12)
project(server_project)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(nlohmann_json REQUIRED)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
pkg_check_modules(PISTACHE REQUIRED IMPORTED_TARGET libpistache)

add_executable(server 
    src/main.cpp 
    src/ServerHandler.cpp
    src/PacketBlockerGateway.cpp
    src/WebSocketService.cpp 
    headers/ServerHandler.h
    headers/PacketBlockerGateway.h
    headers/RequestTypes.h
    headers/WebSocketService.h          
)

target_link_libraries(server PRIVATE 
    PkgConfig::PISTACHE
    ixwebsocket       
    nlohmann_json::nlohmann_json
    ${Boost_LIBRARIES}
    Threads::Threads
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
)

target_include_directories(server PRIVATE 
    ${PISTACHE_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src 
    ${CMAKE_SOURCE_DIR}/headers
    ${Boost_INCLUDE_DIRS}
)

target_compile_options(server PRIVATE 
    -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor 
    -Wold-style-cast -Woverloaded-virtual
    # -Werror 
)
