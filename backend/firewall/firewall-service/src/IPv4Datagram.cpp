#include "../headers/IPv4Datagram.h"

void IPv4Datagram::parse() {
    auto ipPayload = this->payload;
    std::size_t totalPayloadLen =  ipPayload.size();
    if(totalPayloadLen <= 0) throw std::runtime_error("Payload is too short for IPv4\n");
    const uint8_t* ipv4Data = &ipPayload[0];
    if(totalPayloadLen < sizeof(struct iphdr)) {
        throw std::runtime_error("Paylod is too short\n");
    }
    const struct iphdr* ipHeader = reinterpret_cast<const struct iphdr*>(ipv4Data);
    std::size_t headerSize = ipHeader->ihl * 4;
    
    if (totalPayloadLen < headerSize) {
        throw std::runtime_error("Invalid format\n");
    }
    char srcIp[INET_ADDRSTRLEN];
    char destIp[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ipHeader->saddr), srcIp, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ipHeader->daddr), destIp, INET_ADDRSTRLEN);

    
    this->version = static_cast<uint8_t>(ipHeader->version);
    this->header_length = static_cast<uint8_t>(ipHeader->ihl);
    this->service_type = ipHeader->tos;
    this->identification =  ntohs(ipHeader->id);
    this->ttl = ipHeader->ttl;
    this->protocol =  ipHeader->protocol;
    this->checksum =  ntohs(ipHeader->check);
    this->src =  std::string(srcIp);
    this->dest =  std::string(destIp);
}

void IPv4Datagram::parseNext(const uint8_t* nextPayload,std::size_t nextPayloadSize) {
    std::cout << nextPayload << nextPayloadSize;
}