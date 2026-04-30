#include "../headers/TCPHeader.h"
#include <cstdint>
#include <iterator>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdexcept>

void TCPHeader::parse() {
    if (this->payloadSize < sizeof(struct tcphdr)) {
        throw std::runtime_error("Payload too small for basic TCP header (min 20B required)\n");
    }

    const uint8_t* data = this->payloadData;
    auto tcpHeader = reinterpret_cast<const struct tcphdr*>(data);

    uint8_t headerLength = tcpHeader->doff * 4;
/*
    if (this->payloadSize < headerLength) {
        std::cerr << "[TCP Error] Payload size: " << this->payloadSize
                  << ", Header indicates: " << headerLength << " bytes" << std::endl;
        throw std::runtime_error("Payload smaller than TCP header offset indicates (Options truncated?)\n");
    }
*/
    this->sPort      = ntohs(tcpHeader->source);
    this->dPort      = ntohs(tcpHeader->dest);
    this->seqNumber  = ntohl(tcpHeader->seq);
    this->ackNumber  = ntohl(tcpHeader->ack_seq);
    this->windowSize = ntohs(tcpHeader->window);
    this->checkSum   = ntohs(tcpHeader->check);
    this->urgPointer = ntohs(tcpHeader->urg_ptr);
    this->flags      = data[13];

    const uint8_t* nextPayload = data + headerLength;
    std::size_t nextPayloadSize = this->payloadSize - headerLength;

    if (nextPayloadSize > 0) {
        this->parseNext(nextPayload, nextPayloadSize);
    }
}
nlohmann::json  TCPHeader::serialize() const {
    return nlohmann::json{
        {"source_port", sPort},
        {"dest_port", dPort},
        {"seq_number", seqNumber},
        {"ack_number", ackNumber},
        {"flags", flags},
        {"window_size", windowSize},
        {"checksum", checkSum},
        {"urg_pointer", urgPointer}
    };
}
