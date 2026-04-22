#pragma  once
#include "AbstractPDU.h"
#include <cstdint>
#include <ostream>
#include <vector>
#include <netinet/udp.h>
#include <iostream>
#include <netinet/ip.h>    
#include <arpa/inet.h>  
#include <nlohmann/json.hpp>
#include  "DTOS.h"
class UdpDatagram : public AbstractPDU {
public:
    UdpDatagram() : AbstractPDU() {}
    
    void parse() override;
    void parseNext(const uint8_t* /*nextPayload*/, std::size_t /*nextPayloadSize*/) override {};
    [[nodiscard]] uint16_t getSourcePort() const { return this->sPort; }
    [[nodiscard]] uint16_t getDestPort() const { return this->dPort; }
    [[nodiscard]] uint16_t getLen() const { return this->len; }
    [[nodiscard]] uint16_t getChecksum() const { return this->checksum; }


    friend std::ostream& operator<<(std::ostream& o, const UdpDatagram& u ) {
        std::cout << "UDP DATAGRAM\n--------------\n";
        std::cout << "SOURCE PORT: " << u.getSourcePort() << std::endl;
        std::cout << "DEST PORT: " << u.getDestPort() << std::endl;
        std::cout << "LENGTH: " << u.getLen() << std::endl;
        std::cout << "CHECKSUM: " << u.getChecksum() << std::endl;
        return o;
    }

    [[nodiscard]] nlohmann::json serialize() const;

    [[nodiscard]] UdpDTO getDTO() const {
        return UdpDTO {
            sPort,
            dPort,
            len,
            checksum
        };
    }
private:
    uint16_t sPort = 0;
    uint16_t dPort = 0;
    uint16_t len = 0;
    uint16_t checksum = 0;
};