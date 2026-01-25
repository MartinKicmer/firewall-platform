cmake_minimum_required(VERSION 3.12)
project(packet-blocker)

set(CMAKE_CXX_STANDARD 23)

find_package(nlohmann_json REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/headers)

add_executable(packet-blocker
  src/main.cpp
  src/CLIParser.cpp
  ../../firewall-service/src/FilterRule.cpp
  ../../firewall-service/src/PacketParser.cpp
  ../../firewall-service/src/MQConnector.cpp
  ../../firewall-service/src/RuleComparer.cpp
  ../../firewall-service/src/PacketRedirector.cpp
  ../../firewall-service/src/EthernetFrame.cpp
  ../../firewall-service/src/IPv4Datagram.cpp
  headers/CLIParser.h
  ../../firewall-service/headers/MQConnector.h
  ../../firewall-service/headers/FilterRule.h
  ../../firewall-service/headers/PacketParser.h
  ../../firewall-service/headers/RuleComparer.h
  ../../firewall-service/headers/PacketRedirector.h
  ../../firewall-service/headers/EthernetFrame.h
  ../../firewall-service/headers/IPv4Datagram.h
  
)

target_link_libraries(packet-blocker PRIVATE nlohmann_json::nlohmann_json)