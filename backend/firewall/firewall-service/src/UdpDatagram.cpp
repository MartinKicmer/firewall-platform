#include "../headers/UdpDatagram.h"
#include <cstddef>
#include <cstdint>
#include <netinet/udp.h>
#include <stdexcept>

nlohmann::json UdpDatagram::serialize() const {
    return nlohmann::json{
            {"type", "UDP Datagram"},
            {"len", this->len},
            {"checksum", this->checksum},
            {"srcPort", this->sPort},
            {"destPort", this->dPort}
        };
}

void UdpDatagram::parse() {
    if(this->payload.size() <= 0) throw std::runtime_error("Cant parse UDP packet because of not enough size\n");

    const uint8_t* data = &this->payload[0];
    const struct udphdr* udp = reinterpret_cast<const struct udphdr*>(data);

    uint16_t sourcePort = udp->source;
    uint16_t destPort = udp->dest;
    uint16_t len_  = udp->len;
    uint16_t checkSum = udp->check;

    this->sPort = ntohs(udp->source);
    this->dPort = ntohs(udp->dest);
    this->len = ntohs(udp->len);
    this->checksum = ntohs(udp->check);

    const uint8_t* nextPayloadStart = data + sizeof(struct udphdr);
    std::size_t nextPayloadSize = this->payloadLen - sizeof(struct udphdr);

    this->parseNext(nextPayloadStart, nextPayloadSize);
}

void UdpDatagram::parseNext(const uint8_t* nextPayload,std::size_t nextPayloadSize) {
    //std::cout << nextPayload << nextPayloadSize;
}