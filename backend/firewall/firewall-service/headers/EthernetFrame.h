#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <net/ethernet.h> 
#include <cstring>

class EthernetFrame {
public:
    EthernetFrame(
        const std::string& destMAC_,
        const std::string& sourceMAC_,
        uint16_t etherType_,
        const uint8_t* payload_,
        std::size_t payloadLen_
    ) : destinationMAC(destMAC_),sourceMAC(sourceMAC_),etherType(etherType_),payloadLen(payloadLen_) {
        this->payload.assign(payload_,payload_ + payloadLen_);
    }
    
    std::tuple<std::string, std::string> getMACAddresses() const { return std::make_tuple(this->sourceMAC,this->destinationMAC); }
    uint16_t getEtherType() const { return etherType; }
    const std::vector<uint8_t>& getPayload() const { return this->payload; }
private:
    std::string destinationMAC;
    std::string sourceMAC;
    uint16_t etherType;
    std::vector<uint8_t> payload;
    std::size_t payloadLen;
};