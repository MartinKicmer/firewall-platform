#include "../headers/PacketParser.h"


std::string PacketParser::fromBytesToMacString(const unsigned char* bytes) {
    char macStr[18];
    std::snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
                 bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5]);
    return std::string(macStr);
}

std::shared_ptr<EthernetFrame> PacketParser::parseEthernetFrame() {
    const auto& [bytesRead, data] = this->readData;
    if (bytesRead < static_cast<long int>(sizeof(struct ethhdr))) {
        return nullptr;
    }
    const struct ethhdr* ethernetHeader = reinterpret_cast<const struct ethhdr*>(data.data());
    std::string destMAC = PacketParser::fromBytesToMacString(ethernetHeader->h_dest);
    std::string srcMAC  = PacketParser::fromBytesToMacString(ethernetHeader->h_source);
    uint16_t type = ntohs(ethernetHeader->h_proto);

    std::size_t headerSize = sizeof(struct ethhdr);
    std::size_t payloadSize = bytesRead - headerSize;


    const uint8_t* payloadStart = data.data() + headerSize;

    return std::make_shared<EthernetFrame>(destMAC, srcMAC, type, payloadStart,payloadSize);
}

std::shared_ptr<IPv4Datagram> PacketParser::parseIPv4Datagram() {
    auto ethFrame = this->parseEthernetFrame();
    
    if(ethFrame->getEtherType() != 0x0800) {
        return nullptr;
    }

    auto payload = ethFrame->getPayload();
    std::size_t totalPayloadLen =  payload.size();
    if(totalPayloadLen <= 0) return nullptr;
    const uint8_t* ipv4Data = &payload[0];
    if(totalPayloadLen < sizeof(struct iphdr)) {
        return nullptr;
    }
    const struct iphdr* ipHeader = reinterpret_cast<const struct iphdr*>(ipv4Data);
    std::size_t headerSize = ipHeader->ihl * 4;
    
    if (totalPayloadLen < headerSize) {
        return nullptr;
    }
    char srcIp[INET_ADDRSTRLEN];
    char destIp[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ipHeader->saddr), srcIp, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ipHeader->daddr), destIp, INET_ADDRSTRLEN);

    uint16_t totalLenFromHeader = ntohs(ipHeader->tot_len);
    std::size_t actualPayloadSize = (totalLenFromHeader > headerSize) ? (totalLenFromHeader - headerSize) : 0;

    const uint8_t* ipv4PayloadStart = ipv4Data + headerSize;
    
    return std::make_shared<IPv4Datagram>(
        static_cast<uint8_t>(ipHeader->version),
        static_cast<uint8_t>(ipHeader->ihl),
        ipHeader->tos,
        ntohs(ipHeader->id),
        ipHeader->ttl,
        ipHeader->protocol,
        ntohs(ipHeader->check),
        std::string(srcIp),
        std::string(destIp),
        ipv4PayloadStart,
        actualPayloadSize
    );
}