#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <net/ethernet.h> 
#include <cstring>
#include "AbstractPDU.h"
#include "IPv4Datagram.h"
#include <arpa/inet.h>
#include <nlohmann/json.hpp>
#include  "DTOS.h"
class EthernetFrame : public AbstractPDU {
public:
    EthernetFrame() : AbstractPDU(),destinationMAC("none"),sourceMAC("none"),etherType(0) {}

    [[nodiscard]]  std::tuple<std::string, std::string> getMACAddresses() const { return std::make_tuple(this->sourceMAC,this->destinationMAC); }
    [[nodiscard]] uint16_t getEtherType() const { return etherType; }
    void parse() override;
    void parseNext(const uint8_t* nextPayload,std::size_t nextPayloadSize) override;
    static std::string fromBytesToMacString(const unsigned char* mac);
    friend std::ostream& operator<<(std::ostream& o, const EthernetFrame& e) {
        const auto& [source,dest] = e.getMACAddresses();
        std::cout << "Ethernet frame\n----------\n";
        std::cout << "SOURCE MAC: " << source << "\n";
        std::cout << "DESTINATION MAC: " << dest << "\n";
        std::cout << "ETHER TYPE: " << e.getEtherType() << std::endl;

        return o;
    }
    [[nodiscard]] EthernetDTO getDTO() const {
        EthernetDTO dto{};
        std::memcpy(dto.srcMac, this->srcMac, 6);
        std::memcpy(dto.destMac, this->destMac, 6);
        dto.etherType = this->etherType;
        return dto;
    }

    [[nodiscard]] nlohmann::json serialize() const;

    static std::shared_ptr<EthernetFrame> deserialize(const nlohmann::json& j) {
        auto frame = std::make_shared<EthernetFrame>();
        
        if (j.contains("source_mac")) frame->sourceMAC = j["source_mac"];
        if (j.contains("dest_mac")) frame->destinationMAC = j["dest_mac"];
        if (j.contains("ether_type")) frame->etherType = j["ether_type"];
        
        return frame;
    }
private:
    std::string destinationMAC;
    std::string sourceMAC;
    uint8_t srcMac[6];
    uint8_t destMac[6];
    uint16_t etherType;
};