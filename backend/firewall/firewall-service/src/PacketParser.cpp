#include "../headers/PacketParser.h"
#include <memory>



void PacketParser::initParser(const uint8_t* payload, size_t len) {
    this->rawData = payload;
    this->rawLen = len;
    this->initPDUS();
}

void PacketParser::printL2Layer() {
    if (pdus[PduType::ETHERNETFRAME]) {
        std::cout << ethObj << "\n";
    }
}

void PacketParser::printL3Layer() {
    if (pdus[PduType::IPV4DATAGRAM]) {
        std::cout << ipv4Obj << "\n";
    }
}

void PacketParser::printL4Layer() {
    if (pdus[PduType::UDPDATAGRAM]) {
        std::cout << udpObj << "\n";
    } else if (pdus[PduType::TCPPACKET]) {
        std::cout << tcpObj << "\n";
    }
}

void PacketParser::initPDUS() {
    if (rawLen == 0 || rawData == nullptr) return;
    for (int i = 0; i < 4; ++i) this->pdus[i] = nullptr;

    const uint8_t* currentData = rawData;
    size_t currentLen = rawLen;

    if (rawLen < 14) return;

    uint8_t version = (rawData[0] >> 4) & 0x0F;

    if (version == 4 && (rawData[0] & 0x0F) >= 5) {
        this->ipv4Obj.init(rawData, rawLen);
        this->ipv4Obj.parse();
        this->pdus[PduType::IPV4DATAGRAM] = &this->ipv4Obj;
    }
    else {
        return;
    }

    if (this->pdus[PduType::IPV4DATAGRAM] != nullptr) {
        uint8_t protocol = this->ipv4Obj.getProtocol();

        const uint8_t* l4Data = this->ipv4Obj.getPayload() + (this->ipv4Obj.getHeaderLen() * 4);
    	size_t l4Len = this->ipv4Obj.getPayloadSize() - (this->ipv4Obj.getHeaderLen() * 4);

   	 if (protocol == 17) { // UDP
        	this->udpObj.init(l4Data, l4Len);
        	this->udpObj.parse();
        	this->pdus[PduType::UDPDATAGRAM] = &this->udpObj;
    	 }
    	 else if (protocol == 6) { // TCP
        	this->tcpObj.init(l4Data, l4Len);
       		this->tcpObj.parse();
        	this->pdus[PduType::TCPPACKET] = &this->tcpObj;
    	 }
    }
}

[[nodiscard]] CombinedLogRecord PacketParser::getCombinedRecord(bool verdict) const {
    CombinedLogRecord record{};

    record.timestamp = static_cast<uint32_t>(std::time(nullptr));
    record.verdict = verdict ? 1 : 0;

    if (this->pdus[PduType::ETHERNETFRAME]) {
        record.hasl2 = true;
        record.eth = this->ethObj.getDTO();
    }

    if (this->pdus[PduType::IPV4DATAGRAM]) {
        record.hasl3 = true;
        record.ip = this->ipv4Obj.getDTO();
    }

    if (this->pdus[PduType::TCPPACKET]) {
        record.l4Type = L4Type::TCP;
        record.l4.tcp = this->tcpObj.getDTO();
    }
    else if (this->pdus[PduType::UDPDATAGRAM]) {
        record.l4Type = L4Type::UDP;
        record.l4.udp = this->udpObj.getDTO();
    }

    return record;
}
