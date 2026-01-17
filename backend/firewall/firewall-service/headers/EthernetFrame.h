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
        const std::string& destMAC,
        const std::string& sourceMAC,
        uint16_t etherType,
        const uint8_t* payload,
        std::size_t payloadLen
    ) : destinationMAC(destMAC),sourceMAC(sourceMAC),etherType(etherType),payloadLen(payloadLen) {
        this->payload.assign(payload,payload + payloadLen);
    }
    
    std::tuple<std::string, std::string> getMACAddresses() const { return std::make_tuple(this->sourceMAC,this->destinationMAC); }
    uint16_t getEtherType() const { return etherType; }
    const std::vector<uint8_t>& getPayload() const { return this->payload; }
private:
    std::string sourceMAC;
    std::string destinationMAC;
    uint16_t etherType;
    std::size_t payloadLen;
    std::vector<uint8_t> payload;
};