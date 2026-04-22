#pragma once
#include <memory>
#include <tuple>
#include <array>
#include <stdint.h>
#include <vector>
class AbstractPDU {
public:
    AbstractPDU() = default;
    virtual ~AbstractPDU() = default;

    void init(const uint8_t* payload_, std::size_t payloadLen_) {
        this->payloadData = payload_;
        this->payloadSize = payloadLen_;
        this->nextLayer = nullptr;
    }

    virtual void parse() = 0;
    virtual void parseNext(const uint8_t* nextPayload, std::size_t nextPayloadSize) = 0;
    AbstractPDU* getNextLayer() { return nextLayer; }
    void setNextLayer(AbstractPDU* next) { nextLayer = next; }

    [[nodiscard]] const uint8_t* getPayload() const { return payloadData; }
    [[nodiscard]] std::size_t getPayloadSize() const { return payloadSize; }

protected:
    AbstractPDU* nextLayer = nullptr;
    const uint8_t* payloadData = nullptr;
    std::size_t payloadSize = 0;
};