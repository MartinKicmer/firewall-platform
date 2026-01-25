#include "../headers/EthernetFrame.h"

std::string EthernetFrame::fromBytesToMacString(const unsigned char* bytes) {
    char macStr[18];
    std::snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
                 bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5]);
    return std::string(macStr);
}

void EthernetFrame::parse() {
    if (this->payloadLen < static_cast<long int>(sizeof(struct ethhdr))) {
        throw std::runtime_error("Invalid ethernet format\n");
    }
    if(payloadLen <= 0) throw std::runtime_error("Payload is too short\n");
    const uint8_t* data = &this->payload[0];
    const struct ethhdr* ethernetHeader = reinterpret_cast<const struct ethhdr*>(data);
    std::string destMAC = fromBytesToMacString(ethernetHeader->h_dest);
    std::string srcMAC  = fromBytesToMacString(ethernetHeader->h_source);
    uint16_t type = ntohs(ethernetHeader->h_proto);
    std::size_t headerSize = sizeof(struct ethhdr);
    std::size_t payloadSize = this->payloadLen - headerSize;


    const uint8_t* payloadStart = data + headerSize;

    this->destinationMAC = destMAC;
    this->sourceMAC = srcMAC;
    this->etherType = type;

    this->parseNext(payloadStart,payloadSize);

}

void EthernetFrame::parseNext(const uint8_t* nextPayload,std::size_t nextPayloadSize) {
    switch (this->etherType) {
        case 0x0800:
            this->nextLayer = std::make_shared<IPv4Datagram>(nextPayload,nextPayloadSize);
            this->nextLayer->parse();
            break;
        default:
            this->nextLayer = nullptr;
    }
}


nlohmann::json EthernetFrame::serialize() const {
     return nlohmann::json{
            {"type", "EthernetFrame"},
            {"source_mac", sourceMAC},
            {"dest_mac", destinationMAC},
            {"ether_type", etherType}
    };
}
