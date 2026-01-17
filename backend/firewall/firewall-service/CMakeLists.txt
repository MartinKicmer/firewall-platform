cmake_minimum_required(VERSION 3.12)
project(firewallService)

set(CMAKE_CXX_STANDARD 23)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/headers)

add_executable(firewallService
  src/main.cpp
  src/FirewallService.cpp
  src/RawSocket.cpp
  src/PacketParser.cpp
  src/FilterRule.cpp
  src/FilterRuleList.cpp
  headers/FirewallService.h
  headers/RawSocket.h
  headers/EthernetFrame.h
  headers/PacketParser.h
  headers/FilterRule.h
  headers/FilterRuleList.h
  headers/Rules.h
)
