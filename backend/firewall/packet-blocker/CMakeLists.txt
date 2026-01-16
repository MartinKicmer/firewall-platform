cmake_minimum_required(VERSION 3.12)
project(firewallService)

set(CMAKE_CXX_STANDARD 23)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/headers)

add_executable(firewallService
  src/main.cpp
  src/FirewallService.cpp
  headers/FirewallService.h
)
