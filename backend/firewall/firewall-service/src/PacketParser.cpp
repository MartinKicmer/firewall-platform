#include "../headers/PacketParser.h"
#include <memory>

void PacketParser::printL2Layer() {
    auto pdu = this->pdus[PduType::ETHERNETFRAME];
    if(pdu) {
        auto frame = std::dynamic_pointer_cast<EthernetFrame>(pdu);
        if(frame) {
            std::cout << *frame << std::endl;
        }
    }
}

void PacketParser::printL3Layer() {
    auto pdu = this->pdus[PduType::IPV4DATAGRAM];
    if(pdu) {
        auto ipv4datagram = std::dynamic_pointer_cast<IPv4Datagram>(pdu);
        if(ipv4datagram) {
            std::cout << *ipv4datagram << std::endl;
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