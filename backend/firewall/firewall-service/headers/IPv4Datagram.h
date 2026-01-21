#include <cstdint>
#include <string>
#include <vector>
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
class IPv4Datagram {
public:
    IPv4Datagram(
        uint8_t  version,
        uint8_t  header_length,
        uint8_t  service_type,
        uint16_t identification,
        uint8_t  ttl,
        uint8_t  protocol,
        uint16_t checksum,
        std::string source_ip,
        std::string dest_ip,
        const uint8_t* payload,
        std::size_t payloadLen
    )
        : version(version)
        , header_length(header_length)
        , service_type(service_type)
        , total_length(static_cast<uint16_t>((header_length * 4) + payloadLen)) 
        , identification(identification)
        , ttl(ttl)
        , protocol(protocol)
        , checksum(checksum)
        , src(source_ip)
        , dest(dest_ip)
    {
        this->payload.assign(payload,payload + payloadLen);
    }

    friend std::ostream& operator<<(std::ostream& o,const IPv4Datagram& d) {

        o << "---IPv4 Datagram ---" << std::endl;
        o << "Version: " << (int)version << " | IHL: " << (int)header_length << std::endl;
        o << "TOS: " << (int)service_type << " | ID: " << identification << std::endl;
        o<< "Source: " << src << " -> Ddest: " << dest << std::endl;
        o << "Total length: " << total_length << " B" << std::endl;
        o << "TTL: " << (int)ttl << " | Protokol: " << (int)protocol << std::endl;
        o << "Checksum: 0x" << std::hex << checksum << std::dec << std::endl;
        o << "Payload size: " << payload.size() << " B" << std::endl;
        o << "------------------------------" << std::endl;

        return o;
    }

    std::string getSource() const { return src; }
    std::string getDest() const { return dest; }

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