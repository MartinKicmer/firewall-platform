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
class EthernetFrame : public AbstractPDU {
public:
    EthernetFrame(
        const uint8_t* payload_,
        std::size_t payloadLen_
    ) : AbstractPDU(payload_,payloadLen_),destinationMAC("none"),sourceMAC("none"),etherType(0) {}

    std::tuple<std::string, std::string> getMACAddresses() const { return std::make_tuple(this->sourceMAC,this->destinationMAC); }
    uint16_t getEtherType() const { return etherType; }
    const std::vector<uint8_t>& getPayload() const { return this->payload; }

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

    nlohmann::json serialize() const;

    // 2. DESERIALIZACE z JSON (pro načítání pravidel)
    static std::shared_ptr<EthernetFrame> deserialize(const nlohmann::json& j) {
        // Vytvoříme prázdný frame (payload bude prázdný)
        auto frame = std::make_shared<EthernetFrame>(nullptr, 0);
        
        if (j.contains("source_mac")) frame->sourceMAC = j["source_mac"];
        if (j.contains("dest_mac")) frame->destinationMAC = j["dest_mac"];
        if (j.contains("ether_type")) frame->etherType = j["ether_type"];
        
        return frame;
    }
private:
    std::string destinationMAC;
    std::string sourceMAC;
    uint16_t etherType;
};