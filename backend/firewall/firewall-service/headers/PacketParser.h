#pragma once
#include <string>
#include <stdexcept>   
#include <cstring>     
#include <netpacket/packet.h> 
#include <net/ethernet.h>  
#include <net/if.h>        
#include <arpa/inet.h>    
#include <unistd.h>        
#include <iostream>
#include <memory>
#include "EthernetFrame.h"
#include "IPv4Datagram.h"
#include <netinet/ip.h>
#include <array>

class PacketParser {
public:
    static std::string fromBytesToMacString(const unsigned char* mac);
    PacketParser(std::tuple<ssize_t, std::array<uint8_t, BUFSIZ>>& data ) 
    : readData(data) {}
    std::shared_ptr<EthernetFrame> parseEthernetFrame();
    std::shared_ptr<IPv4Datagram>  parseIPv4Datagram();
    
private:
    std::tuple<ssize_t, std::array<uint8_t, BUFSIZ>>& readData;


};