#include "../headers/PacketParser.h"


std::string PacketParser::fromBytesToMacString(const unsigned char* bytes) {
    char macStr[18];
    std::snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
                 bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5]);
    return std::string(macStr);
}

std::shared_ptr<EthernetFrame> PacketParser::parseEthernetFrame() {
    const auto& [bytesRead, data] = this->readData;
    if (bytesRead < sizeof(struct ethhdr)) {
        throw std::runtime_error("Invalid ethernet frame\n");
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