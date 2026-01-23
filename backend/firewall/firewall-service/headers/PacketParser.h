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
#include <unordered_map>

class PacketParser {
public:
    enum PduType {
        ETHERNETFRAME = 0,
        IPV4DATAGRAM = 1
    };
    PacketParser(std::tuple<ssize_t, std::array<uint8_t, BUFSIZ>>& data ) {
        const auto& [payloadLen,payload] = data;
        try {
            auto frame = std::make_shared<EthernetFrame>(payload.data(),payloadLen);
            frame->parse();
            auto ipv4Datagram = frame->getNextLayer();
            this->pdus[PduType::ETHERNETFRAME] = frame;
            if(!ipv4Datagram) this->pdus[PduType::ETHERNETFRAME] = nullptr;
            else this->pdus[PduType::IPV4DATAGRAM] = ipv4Datagram;

        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }

    void printL2Layer() {
        auto pdu = this->pdus[PduType::ETHERNETFRAME];
        if(pdu) {
            auto frame = std::dynamic_pointer_cast<EthernetFrame>(pdu);
            if(frame) {
                std::cout << *frame << std::endl;
            }
        }
    }
    void printL3Layer() {
        auto pdu = this->pdus[PduType::IPV4DATAGRAM];
        if(pdu) {
            auto ipv4datagram = std::dynamic_pointer_cast<IPv4Datagram>(pdu);
            if(ipv4datagram) {
                std::cout << *ipv4datagram << std::endl;
            }
        }
    }

    [[nodiscard]] std::shared_ptr<EthernetFrame> getEthernetFrame() { 
        return std::static_pointer_cast<EthernetFrame>(this->pdus[PduType::ETHERNETFRAME]);
    }
    [[nodiscard]] std::shared_ptr<IPv4Datagram> getIPv4Datagram()   {   
        return std::static_pointer_cast<IPv4Datagram>(this->pdus[PduType::IPV4DATAGRAM]);
     }
    
private:
    std::unordered_map<int,std::shared_ptr<AbstractPDU>> pdus;
};