#include "../headers/IPv4Datagram.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <netinet/in.h>
#include <netinet/ip.h>
void IPv4Datagram::parse() {
    if (this->payloadSize <= 0) throw std::runtime_error("Empty payload");

    const uint8_t* ipv4Data = this->payloadData;
    std::size_t totalPayloadLen = this->payloadSize;

    if (totalPayloadLen < sizeof(struct iphdr)) {
        throw std::runtime_error("Payload too short for IP header");
    }

    auto ipHeader = reinterpret_cast<const struct iphdr*>(ipv4Data);
    
    if (ipHeader->ihl < 5) {
        throw std::runtime_error("Invalid IP header length (IHL < 5)");
    }

    std::size_t headerSize = ipHeader->ihl * 4;
    
    if (totalPayloadLen < headerSize) {
        throw std::runtime_error("IP packet truncated (total len < header size)");
    }

    this->total_length = ntohs(ipHeader->tot_len);
    
    char srcIpBuf[INET_ADDRSTRLEN];
    char destIpBuf[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ipHeader->saddr), srcIpBuf, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ipHeader->daddr), destIpBuf, INET_ADDRSTRLEN);

    this->srcIp = ipHeader->saddr;
    this->destIp = ipHeader->daddr;
    this->version = ipHeader->version;
    this->header_length = ipHeader->ihl;
    this->service_type = ipHeader->tos;
    this->identification = ntohs(ipHeader->id);
    this->ttl = ipHeader->ttl;
    this->protocol = ipHeader->protocol;
    this->checksum = ntohs(ipHeader->check);
    this->src = std::string(srcIpBuf);
    this->dest = std::string(srcIpBuf);

    const uint8_t* nextPayLoadStart = ipv4Data + headerSize;
    std::size_t nextPayloadSize = totalPayloadLen - headerSize;

    this->parseNext(nextPayLoadStart, nextPayloadSize);
}

void IPv4Datagram::parseNext(const uint8_t* nextPayload, std::size_t nextPayloadSize) {
    if (!this->nextLayer) return;

    switch (this->protocol) {
    case IPPROTO_UDP:
        this->nextLayer->init(nextPayload, nextPayloadSize);
        this->nextLayer->parse();
        break;

    case IPPROTO_TCP:
        this->nextLayer->init(nextPayload, nextPayloadSize);
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
    auto datagram = std::make_shared<IPv4Datagram>();
        
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