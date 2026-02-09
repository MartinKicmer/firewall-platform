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
        if(auto tcp = std::dynamic_pointer_cast<TCPHeader>(pdu)) {
             std::cout << *tcp << std::endl;
        }
    }
}


void PacketParser::initPDUS() {
    const auto& [payloadLen, payload] = this->data;
    if (payloadLen == 0) return;

    for(int i = 0; i <= 3; ++i) this->pdus[i] = nullptr;

    const uint8_t* rawData = payload.data();
    std::shared_ptr<AbstractPDU> l3Layer = nullptr;

    try {

        if (rawData[0] == 0x45 || (rawData[0] & 0xF0) == 0x60) {
            auto ipv4 = std::make_shared<IPv4Datagram>(rawData, payloadLen);
            ipv4->parse();
            pdus[PduType::IPV4DATAGRAM] = ipv4;
            l3Layer = ipv4;
        } 
        else {
            auto frame = std::make_shared<EthernetFrame>(rawData, payloadLen);
            frame->parse(); 
            pdus[PduType::ETHERNETFRAME] = frame;
            l3Layer = frame->getNextLayer();
            
            if (auto ipv4 = std::dynamic_pointer_cast<IPv4Datagram>(l3Layer)) {
                pdus[PduType::IPV4DATAGRAM] = ipv4;
            }
        }

        if (l3Layer) {
            auto l4Layer = l3Layer->getNextLayer();
            if (l4Layer) {
                if (auto udp = std::dynamic_pointer_cast<UdpDatagram>(l4Layer)) {
                    pdus[PduType::UDPDATAGRAM] = udp;
                }
                else if (auto tcp = std::dynamic_pointer_cast<TCPHeader>(l4Layer)) {
                    pdus[PduType::TCPPACKET] = tcp;
                }
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Packet type wasnt detected\n";
    }
}