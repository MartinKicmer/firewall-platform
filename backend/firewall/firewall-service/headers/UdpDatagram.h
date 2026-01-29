#pragma  once
#include "AbstractPDU.h"
#include <cstdint>
#include <vector>
#include <netinet/udp.h>
#include <iostream>
class UdpDatagram : public AbstractPDU {
public:
    UdpDatagram(
        const uint8_t* payload_,
        std::size_t payloadLen_) : AbstractPDU(payload_,payloadLen_) {}
    
    void parse() override;
    void parseNext(const uint8_t* nextPayload,std::size_t nextPayloadSize) override;
    uint16_t getSourcePort() const { return this->sPort; }
    uint16_t getDestPort() const { return this->dPort; }
    uint16_t getLen() const { return this->len; }
    uint16_t getChecksum() const { return this->checksum; }
private:
    uint16_t sPort;
    uint16_t dPort;
    uint16_t len;
    uint16_t checksum;
};