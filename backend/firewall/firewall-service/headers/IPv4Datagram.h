#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include "AbstractPDU.h"
#include <netinet/ip.h>    
#include <arpa/inet.h>     
#include <nlohmann/json.hpp>
#include "UdpDatagram.h"
class IPv4Datagram : public AbstractPDU {
public:
    IPv4Datagram(
        const uint8_t* payload_,
        std::size_t payloadLen_
    )
        : AbstractPDU(payload_,payloadLen_)
        , version(0)
        , header_length(0)
        , service_type(0)
        , total_length(0) 
        , identification(0)
        , ttl(0)
        , protocol(0)
        , checksum(0)
        , src("none")
        , dest("none")
    {}

    void parse() override;
    void parseNext(const uint8_t* nextPayload,std::size_t nextPayloadSize) override;

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

    nlohmann::json serialize() const;
    static std::shared_ptr<IPv4Datagram> deserialize(const nlohmann::json& j);

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
};