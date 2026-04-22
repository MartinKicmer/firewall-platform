//
// Created by andrew on 4/22/26.
//

#pragma once
#include <cstdint>
struct EthernetDTO {
    uint8_t srcMac[6];
    uint8_t destMac[6];
    uint16_t etherType;
};

struct IPv4DTO {
    uint32_t srcIp;
    uint32_t destIp;
    uint8_t version;
    uint8_t header_length;
    uint8_t service_type;
    uint16_t total_length;
    uint16_t identification;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
};

struct TcpDTO {
    short sPort = 0;
    short dPort = 0;
    uint32_t seqNumber = 0;
    uint32_t ackNumber = 0;
    uint8_t  flags = 0;
    uint16_t windowSize = 0;
    uint16_t checkSum = 0;
    uint16_t urgPointer = 0;
};

struct UdpDTO {
    uint16_t sPort;
    uint16_t dPort;
    uint16_t len;
    uint16_t checksum;
};

enum class L4Type : uint8_t { NONE = 0, TCP = 1, UDP = 2 };

struct CombinedLogRecord {
    uint32_t timestamp = 0;
    uint8_t verdict = 0;
    bool hasl2 = false;
    bool hasl3 = false;
    EthernetDTO eth{};
    IPv4DTO ip{};
    L4Type l4Type = L4Type::NONE;
    union {
        TcpDTO tcp{};
        UdpDTO udp;
    } l4;
};