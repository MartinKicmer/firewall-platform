#include "../headers/IPv4Datagram.h"
#include <memory>

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
    switch (this->protocol) {
        case IPPROTO_UDP:
            this->nextLayer = std::make_shared<UdpDatagram>(nextPayload,nextPayloadSize);
            this->nextLayer->parse();
            break;
        default:
            this->nextLayer = nullptr;
            break;
    }
}

nlohmann::json IPv4Datagram::serialize() const {
    return nlohmann::json{
            {"type", "IPv4Datagram"},
            {"version", version},
            {"header_len", header_length},
            {"tos", service_type},
            {"total_len", total_length},
            {"id", identification},
            {"ttl", ttl},
            {"protocol", protocol},
            {"checksum", checksum},
            {"src", src},
            {"dest", dest}
        };
}

std::shared_ptr<IPv4Datagram> IPv4Datagram::deserialize(const nlohmann::json &j) {
    auto datagram = std::make_shared<IPv4Datagram>(nullptr, 0);
        
    if (j.contains("version")) datagram->version = j["version"];
    if (j.contains("header_len")) datagram->header_length = j["header_len"];
    if (j.contains("tos")) datagram->service_type = j["tos"];
    if (j.contains("total_len")) datagram->total_length = j["total_len"];
    if (j.contains("id")) datagram->identification = j["id"];
    if (j.contains("ttl")) datagram->ttl = j["ttl"];
    if (j.contains("protocol")) datagram->protocol = j["protocol"];
    if (j.contains("checksum")) datagram->checksum = j["checksum"];
    if (j.contains("src")) datagram->src = j["src"];
    if (j.contains("dest")) datagram->dest = j["dest"];
    
    return datagram;
}