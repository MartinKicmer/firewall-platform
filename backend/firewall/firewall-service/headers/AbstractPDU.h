#pragma once
#include <memory>
#include <tuple>
#include <array>
#include <stdint.h>
#include <vector>
class AbstractPDU {
public:
    AbstractPDU(
        const uint8_t* payload_,
        std::size_t payloadLen_
    ) {
        this->payloadLen = payloadLen_;
        this->payload.assign(payload_,payload_ + payloadLen_);
    }

    virtual ~AbstractPDU() = default;
    virtual void parse() = 0;
    virtual void parseNext(const uint8_t* nextPayload,std::size_t nextPayloadSize) = 0;
    std::shared_ptr<AbstractPDU> getNextLayer() { return this->nextLayer; }

protected:
    std::shared_ptr<AbstractPDU> nextLayer;
    std::size_t payloadLen;
    std::vector<uint8_t> payload;

};