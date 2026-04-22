#pragma once
#include <cstddef>
#include <string>
#include <vector>
#include <memory>
#include <tuple>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <unordered_map>
#include "AbstractPDU.h"
#include "IPv4Datagram.h"
#include "TCPHeader.h"
#include "EthernetFrame.h"
#include "DTOS.h"
#include <optional>
class PacketParser {
public:
    enum PduType {
        ETHERNETFRAME = 0,
        IPV4DATAGRAM = 1,
        UDPDATAGRAM = 2,
        TCPPACKET = 3
    };

    PacketParser()
        : rawData(nullptr), rawLen(0) {}

    void initParser(const uint8_t* payload, size_t len);
    void printL2Layer();
    void printL3Layer();
    void printL4Layer();

    void initPDUS();
    [[nodiscard]] CombinedLogRecord getCombinedRecord(bool verdict) const;

    [[nodiscard]] EthernetFrame* getEthernetFrame() {
        return (pdus[ETHERNETFRAME] != nullptr) ? &ethObj : nullptr;
    }

    [[nodiscard]] IPv4Datagram* getIPv4Datagram() {
        return (pdus[IPV4DATAGRAM] != nullptr) ? &ipv4Obj : nullptr;
    }

    [[nodiscard]] UdpDatagram* getUDPDatagram() {
        return (pdus[UDPDATAGRAM] != nullptr) ? &udpObj : nullptr;
    }

    [[nodiscard]] TCPHeader* getTCPPACKET() {
        return (pdus[TCPPACKET] != nullptr) ? &tcpObj : nullptr;
    }

    [[nodiscard]] std::optional<EthernetDTO> getL2DTO() const {
        if (pdus[PduType::ETHERNETFRAME]) {
            return dynamic_cast<EthernetFrame*>(pdus[PduType::ETHERNETFRAME])->getDTO();
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<IPv4DTO> getL3DTO() const {
        if (pdus[PduType::IPV4DATAGRAM]) {
            return dynamic_cast<IPv4Datagram*>(pdus[PduType::IPV4DATAGRAM])->getDTO();
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<TcpDTO> getL4TcpDTO() const {
        if (pdus[PduType::TCPPACKET]) {
            return dynamic_cast<TCPHeader*>(pdus[PduType::TCPPACKET])->getDTO();
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<UdpDTO> getL4UdpDTO() const {
        if (pdus[PduType::UDPDATAGRAM]) {
            return dynamic_cast<UdpDatagram*>(pdus[PduType::UDPDATAGRAM])->getDTO();
        }
        return std::nullopt;
    }
    
private:

    const uint8_t* rawData;
    size_t rawLen;

    EthernetFrame ethObj;
    IPv4Datagram  ipv4Obj;
    UdpDatagram   udpObj;
    TCPHeader     tcpObj;

    AbstractPDU* pdus[4];
};