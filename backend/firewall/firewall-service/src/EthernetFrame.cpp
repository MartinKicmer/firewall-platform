#include "../headers/EthernetFrame.h"

std::string EthernetFrame::fromBytesToMacString(const unsigned char* bytes) {
    char macStr[18];
    std::snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
                 bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5]);
    return macStr;
}

void EthernetFrame::parse() {
    if (this->payloadSize < static_cast<long int>(sizeof(struct ethhdr))) {
        throw std::runtime_error("Invalid ethernet format\n");
    }
    if(this->payloadSize <= 0) throw std::runtime_error("Payload is too short\n");
    const uint8_t* data = this->payloadData;
    auto ethernetHeader = reinterpret_cast<const struct ethhdr*>(data);
    std::memcpy(this->srcMac,ethernetHeader->h_source,6);
    std::memcpy(this->destMac,ethernetHeader->h_dest,6);
    std::string destMAC = fromBytesToMacString(ethernetHeader->h_dest);
    std::string srcMAC  = fromBytesToMacString(ethernetHeader->h_source);
    uint16_t type = ntohs(ethernetHeader->h_proto);
    std::size_t headerSize = sizeof(struct ethhdr);
    std::size_t payloadSize_ = this->payloadSize - headerSize;


    const uint8_t* payloadStart = data + headerSize;

    this->destinationMAC = destMAC;
    this->sourceMAC = srcMAC;
    this->etherType = type;

    this->parseNext(payloadStart,payloadSize_);

}

void EthernetFrame::parseNext(const uint8_t* nextPayload,std::size_t nextPayloadSize) {
    if (this->nextLayer) {
        switch (this->etherType) {
        case 0x0800:
            this->nextLayer->init(nextPayload, nextPayloadSize);
            this->nextLayer->parse();
            break;
        default:
            this->nextLayer = nullptr;
            break;
        }
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
