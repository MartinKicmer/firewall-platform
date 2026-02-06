#pragma once
#include <cstdint>
#include <netinet/tcp.h>
#include "AbstractPDU.h"
#include <arpa/inet.h>

class TCPHeader : public AbstractPDU {
public:
    TCPHeader(const uint8_t* payload_, std::size_t payloadLen_) 
        : AbstractPDU(payload_, payloadLen_) {}
    uint16_t getSourcePort() const { return sPort; }
    uint16_t getDestPort() const { return dPort; }
    uint32_t getSeqNumber() const { return seqNumber; }
    uint32_t getAckNumber() const { return ackNumber; }
    uint16_t getWindowSize() const { return windowSize; }
    uint8_t  getFlags() const { return flags; }

    bool isSyn() const { return flags & TH_SYN; }
    bool isAck() const { return flags & TH_ACK; }
    bool isFin() const { return flags & TH_FIN; }

    void parse() override;
    void parseNext(const uint8_t* nextPayload,std::size_t nextPayloadSize) override;
private:
    uint16_t sPort = 0;
    uint16_t dPort = 0;
    uint32_t seqNumber = 0;
    uint32_t ackNumber = 0;
    uint8_t  flags = 0;
    uint16_t windowSize = 0;
    uint16_t checkSum = 0;
    uint16_t urgPointer = 0;
};