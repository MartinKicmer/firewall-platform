#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include "AbstractPDU.h"
class IPv4Datagram : public AbstractPDU {
public:
    IPv4Datagram(
        uint8_t  version_,
        uint8_t  header_length_,
        uint8_t  service_type_,
        uint16_t identification_,
        uint8_t  ttl_,
        uint8_t  protocol_,
        uint16_t checksum_,
        std::string source_ip_,
        std::string dest_ip_,
        const uint8_t* payload_,
        std::size_t payloadLen_
    )
        : AbstractPDU(),version(version_)
        , header_length(header_length_)
        , service_type(service_type_)
        , total_length(static_cast<uint16_t>((header_length * 4) + payloadLen_)) 
        , identification(identification_)
        , ttl(ttl_)
        , protocol(protocol_)
        , checksum(checksum_)
        , src(source_ip_)
        , dest(dest_ip_)
    {
        this->payload.assign(payload_,payload_ + payloadLen_);
    }

    friend std::ostream& operator<<(std::ostream& o,const IPv4Datagram& d) {

        o << "---IPv4 Datagram ---" << std::endl;
        o << "Version: " << static_cast<int>(d.getVersion()) << " | IHL: " << static_cast<int>(d.getHeaderLen()) << std::endl;
        o << "TOS: " << static_cast<int>(d.getServiceType()) << " | ID: " << d.getIdentification() << std::endl;
        o<< "Source: " << d.src << " -> Ddest: " << d.dest << std::endl;
        o << "Total length: " << d.getTotalLength() << " B" << std::endl;
        o << "TTL: " << static_cast<int>(d.getTTL()) << " | Protokol: " << static_cast<int>(d.getProtocol()) << std::endl;
        o << "Checksum: 0x" << std::hex << d.getChecksum() << std::dec << std::endl;
        o << "Payload size: " << d.getPayload().size(
        ) << " B" << std::endl;
        o << "------------------------------" << std::endl;

        return o;
    }

    [[nodiscard]]  std::string getSource() const { return this->src; }
    [[nodiscard]]  std::string getDest() const { return this->dest; }
    [[nodiscard]]  uint8_t getVersion() const { return this->version; }
    [[nodiscard]]  uint8_t getHeaderLen() const { return this->header_length; }
    [[nodiscard]]  uint8_t getServiceType() const { return this->service_type; }
    [[nodiscard]]  uint16_t getTotalLength() const { return this->total_length; }
    [[nodiscard]]  uint16_t getIdentification() const { return this->identification; }
    [[nodiscard]]  uint8_t  getTTL() const { return this->ttl; }
    [[nodiscard]]  uint8_t  getProtocol() const { return this->protocol; }
    [[nodiscard]]  uint16_t getChecksum() const { return this->checksum; }
    [[nodiscard]]  const  std::string& getSRC() const { return this->src; }
    [[nodiscard]]  const  std::string& getDST() const { return this->dest; }
    [[nodiscard]]  const   std::vector<uint8_t>& getPayload() const { return this->payload; }

private:
    uint8_t version;        
    uint8_t header_length;  
    uint8_t service_type;   
    uint16_t total_length;
    uint16_t identification;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    std::string src;
    std::string dest;
    std::vector<uint8_t> payload; 
};