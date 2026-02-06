#pragma once
#include <cstdint>
#include <netinet/tcp.h>
#include "AbstractPDU.h"
#include <arpa/inet.h>
#include <iostream>
#include <ostream>

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
    uint16_t getChecksum() const { return this->checkSum; }
    bool isSyn() const { return flags & TH_SYN; }
    bool isAck() const { return flags & TH_ACK; }
    bool isFin() const { return flags & TH_FIN; }

    void parse() override;
    void parseNext(const uint8_t* nextPayload,std::size_t nextPayloadSize) override;

    friend std::ostream& operator<<(std::ostream& o,const TCPHeader& hdr) {
        o << "TCP Header\n------------\n"
        << "  Source Port:      " << hdr.getSourcePort() << "\n"
        << "  Destination Port: " << hdr.getDestPort() << "\n"
        << "  Sequence Number:  " << hdr.getSeqNumber() << "\n"
        << "  Ack Number:       " << hdr.getAckNumber() << "\n"
        << "  Window Size:      " << hdr.getWindowSize() << "\n"
        << "  Flags:            0x" << std::hex << static_cast<int>(hdr.getFlags()) << std::dec << " (";
        
        bool first = true;
        auto printFlag = [&](const char* name, bool set) {
            if (set) {
                if (!first) o << ", ";
                o << name;
                first = false;
            }
        };

        printFlag("SYN", hdr.isSyn());
        printFlag("ACK", hdr.isAck());
        printFlag("FIN", hdr.isFin());
        printFlag("RST", hdr.getFlags() & TH_RST);
        printFlag("PSH", hdr.getFlags() & TH_PUSH);
        printFlag("URG", hdr.getFlags() & TH_URG);
        
        o << ")\n"
        << "  Checksum:         0x" << std::hex << hdr.getChecksum() << std::dec << "\n"
        << "==================";
        return o;
    }
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