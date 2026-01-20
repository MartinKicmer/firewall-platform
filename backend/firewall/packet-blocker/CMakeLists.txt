cmake_minimum_required(VERSION 3.12)
project(packet-blocker)

set(CMAKE_CXX_STANDARD 23)


include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/headers)

add_executable(packet-blocker
  src/main.cpp
  src/CLIParser.cpp
  ../../firewall-service/src/FilterRule.cpp
  ../../firewall-service/src/PacketParser.cpp
  ../../firewall-service/src/MQConnector.cpp
  headers/CLIParser.h
  ../../firewall-service/headers/MQConnector.h
  ../../firewall-service/headers/FilterRule.h
   ../../firewall-service/headers/PacketParser.h
)

