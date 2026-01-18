cmake_minimum_required(VERSION 3.12)
project(packet-blocker)

set(CMAKE_CXX_STANDARD 23)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/headers)

add_executable(packet-blocker
  src/main.cpp
  src/CLIParser.cpp
  headers/CLIParser.h
)
