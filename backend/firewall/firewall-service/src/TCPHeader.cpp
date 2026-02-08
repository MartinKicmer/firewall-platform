#include "../headers/TCPHeader.h"
#include <cstdint>
#include <iterator>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdexcept>

void TCPHeader::parse() {
 
    if(this->payload.size() <= 0) throw std::runtime_error("Payload too small for TCP header\n");
    const uint8_t* data = &this->payload[0];
    const struct tcphdr* tcpHeader = reinterpret_cast<const struct tcphdr*>(data);


    this->sPort = ntohs(tcpHeader->source);
    this->dPort = ntohs(tcpHeader->dest);
    this->seqNumber  = ntohl(tcpHeader->seq);    
    this->ackNumber  = ntohl(tcpHeader->ack_seq);
    this->windowSize = ntohs(tcpHeader->window);
    this->checkSum   = ntohs(tcpHeader->check);
    this->urgPointer = ntohs(tcpHeader->urg_ptr);
    this->flags = data[13];
    uint8_t headerLength = tcpHeader->doff * 4;

    if (this->payload.size() < headerLength) {
        throw std::runtime_error("Payload smaller than TCP header offset indicates\n");
    }
    const uint8_t* nextPayload = data + sizeof(struct tcphdr);
    std::size_t nextPayloadSize = this->payload.size() - sizeof(struct tcphdr);

    this->parseNext(nextPayload, nextPayloadSize);

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

void TCPHeader::parseNext(const uint8_t* nextPayload,std::size_t nextPayloadSize) {
    return;
}

