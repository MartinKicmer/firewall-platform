#include "../headers/PacketParser.h"
#include <memory>

void PacketParser::printL2Layer(PacketParser::PduType type) {
    auto pdu = this->pdus[type];
    if(pdu) {
        auto frame = std::dynamic_pointer_cast<EthernetFrame>(pdu);
        if(frame) {
            std::cout << *frame << std::endl;
        }
    }
}

void PacketParser::printL3Layer(PacketParser::PduType type) {
    auto pdu = this->pdus[type];
    if(pdu) {
        if(auto ipv4Datagram = std::dynamic_pointer_cast<IPv4Datagram>(pdu)) {
             std::cout << *ipv4Datagram << std::endl;
        }
    }
}

void PacketParser::printL4Layer(PacketParser::PduType type) {
    auto pdu = this->pdus[type];
    if(pdu) {
        if(auto udpDatagram = std::dynamic_pointer_cast<UdpDatagram>(pdu)) {
             std::cout << *udpDatagram << std::endl;
        }
    }
}


void PacketParser::initPDUS() {
    const auto& [payloadLen, payload] = this->data;
    
    for(int i = 0; i < 3; ++i) this->pdus[i] = nullptr;

    try {
        auto frame = std::make_shared<EthernetFrame>(payload.data(), payloadLen);
        frame->parse();
        pdus[PduType::ETHERNETFRAME] = frame;

        auto L3PDU = frame->getNextLayer();
        if (!L3PDU) return; 

        if (auto ipv4 = std::dynamic_pointer_cast<IPv4Datagram>(L3PDU)) {
            pdus[PduType::IPV4DATAGRAM] = ipv4;
            auto L4PDU = L3PDU->getNextLayer();
            if (L4PDU) {
                if (auto udp = std::dynamic_pointer_cast<UdpDatagram>(L4PDU)) {
                    pdus[PduType::UDPDATAGRAM] = udp;
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Parser Error: " << e.what() << std::endl;
    }
}