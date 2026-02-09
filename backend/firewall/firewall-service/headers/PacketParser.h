#pragma once
#include <cstddef>
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
#include "AbstractPDU.h"
#include "UdpDatagram.h"
#include <unordered_map>
#include "TCPHeader.h"

class PacketParser {
public:
    enum PduType {
        ETHERNETFRAME = 0,
        IPV4DATAGRAM = 1,
        UDPDATAGRAM = 2,
        TCPPACKET = 3
    };
    PacketParser(const std::tuple<ssize_t, std::array<uint8_t, BUFSIZ>>& data_ ) : data(data_) {
        this->initPDUS();
    }

    void printL2Layer(PacketParser::PduType type);
    void printL3Layer(PacketParser::PduType type);
    void printL4Layer(PacketParser::PduType type);
    void initPDUS();

    [[nodiscard]] std::shared_ptr<EthernetFrame> getEthernetFrame() { 
        return std::static_pointer_cast<EthernetFrame>(this->pdus[PduType::ETHERNETFRAME]);
    }
    [[nodiscard]] std::shared_ptr<IPv4Datagram> getIPv4Datagram()   {   
        return std::static_pointer_cast<IPv4Datagram>(this->pdus[PduType::IPV4DATAGRAM]);
    }

    [[nodiscard]] std::shared_ptr<UdpDatagram> getUDPDatagram() {
        return std::static_pointer_cast<UdpDatagram>(this->pdus[PduType::UDPDATAGRAM]);
    }
    [[nodiscard]] std::shared_ptr<TCPHeader> getTCPPACKET() {
        return std::static_pointer_cast<TCPHeader>(this->pdus[PduType::TCPPACKET]);
    }  
    
private:
    std::unordered_map<int,std::shared_ptr<AbstractPDU>> pdus;
    const std::tuple<ssize_t, std::array<uint8_t, BUFSIZ>>& data;
};