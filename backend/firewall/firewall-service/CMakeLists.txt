cmake_minimum_required(VERSION 3.12)
project(firewallService)

set(CMAKE_CXX_STANDARD 23)

find_package(nlohmann_json REQUIRED)
find_package(SQLite3 REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/headers)

add_executable(firewallService
  src/main.cpp
  src/FirewallService.cpp
  src/RawSocket.cpp
  src/PacketParser.cpp
  src/FilterRule.cpp
  src/FilterRuleList.cpp
  src/MQConnector.cpp
  src/RuleComparer.cpp
  src/EthernetFrame.cpp
  src/IPv4Datagram.cpp
  src/PacketRedirector.cpp
  src/FilterRuleLogger.cpp
  src/UdpDatagram.cpp
  src/PacketblockerGateway.cpp
  headers/IPv4Datagram.h
  headers/AbstractPDU.h
  headers/RuleComparer.h
  headers/FirewallService.h
  headers/PacketblockerGateway.h
  headers/RawSocket.h
  headers/EthernetFrame.h
  headers/PacketParser.h
  headers/FilterRule.h
  headers/FilterRuleList.h
  headers/Rules.h
  headers/MQConnector.h
  headers/PacketRedirector.h
  headers/FilterRuleLogger.h
  headers/UdpDatagram.h
)
target_compile_options(firewallService PRIVATE      
        -Wextra         
        -Wpedantic     
        -Wshadow        
        -Wnon-virtual-dtor 
        -Wold-style-cast         
        -Woverloaded-virtual 
        -Werror         
    )
target_link_libraries(firewallService PRIVATE 
    nlohmann_json::nlohmann_json 
    rt 
    pthread 
    SQLite::SQLite3
)