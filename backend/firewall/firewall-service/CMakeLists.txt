cmake_minimum_required(VERSION 3.12)
project(firewallService)

set(CMAKE_CXX_STANDARD 23)

find_package(nlohmann_json REQUIRED)


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
  headers/IPv4Datagram.h
  headers/AbstractPDU.h
  headers/RuleComparer.h
  headers/FirewallService.h
  headers/RawSocket.h
  headers/EthernetFrame.h
  headers/PacketParser.h
  headers/FilterRule.h
  headers/FilterRuleList.h
  headers/Rules.h
  headers/MQConnector.h
)
target_compile_options(firewallService PRIVATE 
        -Wall         
        -Wextra         
        -Wpedantic     
        -Wshadow        
        -Wnon-virtual-dtor 
        -Wold-style-cast   
        -Wunused        
        -Woverloaded-virtual 
        -Werror         
    )
target_link_libraries(firewallService PRIVATE nlohmann_json::nlohmann_json)