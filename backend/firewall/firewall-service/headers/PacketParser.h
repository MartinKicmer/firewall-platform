#pragma once
#include <cstddef>
#include <string>
#include <vector>
#include <memory>
#include <tuple>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <unordered_map>
#include "AbstractPDU.h"
#include "IPv4Datagram.h"
#include "TCPHeader.h"
#include "EthernetFrame.h"

class PacketParser {
public:
    enum PduType {
        ETHERNETFRAME = 0,
        IPV4DATAGRAM = 1,
        UDPDATAGRAM = 2,
        TCPPACKET = 3
    };

    PacketParser(const uint8_t* payload, size_t len) 
        : rawData(payload), rawLen(len) {
        this->initPDUS();
    }

    void printL2Layer(PacketParser::PduType type);
    void printL3Layer(PacketParser::PduType type);
    void printL4Layer(PacketParser::PduType type);
    void initPDUS();
    [[nodiscard]] std::shared_ptr<EthernetFrame> getEthernetFrame() { 
        return std::static_pointer_cast<EthernetFrame>(this->pdus[PduType::ETHERNETFRAME]);
    }
    [[nodiscard]] std::shared_ptr<IPv4Datagram> getIPv4Datagram() {   
        return std::static_pointer_cast<IPv4Datagram>(this->pdus[PduType::IPV4DATAGRAM]);
    }
    [[nodiscard]] std::shared_ptr<UdpDatagram> getUDPDatagram() {
        return std::static_pointer_cast<UdpDatagram>(this->pdus[PduType::UDPDATAGRAM]);
    }
    [[nodiscard]] std::shared_ptr<TCPHeader> getTCPPACKET() {
        return std::static_pointer_cast<TCPHeader>(this->pdus[PduType::TCPPACKET]);
    }  
    
private:
    std::unordered_map<int, std::shared_ptr<AbstractPDU>> pdus;

    const uint8_t* rawData;
    size_t rawLen;
};