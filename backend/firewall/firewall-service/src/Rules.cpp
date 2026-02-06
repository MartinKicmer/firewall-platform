#include "../headers/Rules.h"


bool L2Rule::match(std::shared_ptr<PacketParser> parser){
    auto frame = parser->getEthernetFrame();
    if(!frame) return false;
    const auto& [sourceMAC,destMAC] = frame->getMACAddresses();
    bool sourceMatch = (this->source == "none" || sourceMAC == this->source);
    bool destMatch   = (this->dest == "none" || destMAC == this->dest);
    return (sourceMatch && destMatch);
}


bool L3Rule::match(std::shared_ptr<PacketParser> parser)  {
    auto datagram = parser->getIPv4Datagram();
    if(!datagram) return false;
    bool sourceMatch = (RuleComparer::matchIP(datagram->getSource(), this->source));
    bool destMatch = (RuleComparer::matchIP(datagram->getDest(), this->dest));
    bool protoMatch = (this->protocol == -1 || datagram->getProtocol() == this->protocol );
    bool tosMatch = (this->tos == -1 || datagram->getServiceType() == this->tos);
    bool ttlMatch = ((this->ttlMin == -1 || this->ttlMax == -1 ) || (datagram->getTTL() >= this->ttlMin && datagram->getTTL() <= this->ttlMax) );
    return (sourceMatch && destMatch && protoMatch && tosMatch && ttlMatch);
}

bool L4SimpleRule::match(std::shared_ptr<PacketParser> parser) {
    auto udpDatagram = parser->getUDPDatagram();
    if(!udpDatagram) return false;

    bool sourcePortMatch = (this->sPort == -1 || this->sPort == udpDatagram->getSourcePort() );
    bool destPortMatch = (this->dPort == -1 || this->dPort == udpDatagram->getDestPort());

    return (sourcePortMatch && destPortMatch);
}

bool L4TcpRule::match(std::shared_ptr<PacketParser> parser) {
    auto tcp = parser->getTCPPACKET();
    if(!tcp) return false;

    bool sourcePortMatch = (this->sPort == -1 || this->sPort == tcp->getSourcePort() );
    bool destPortMatch = (this->dPort == -1 || this->dPort == tcp->getDestPort());
    bool windowSizeMatch = ((this->minWindow == -1 || this->maxWindow == -1) || (tcp->getWindowSize() >= this->minWindow && tcp->getWindowSize() <= this->maxWindow));
    bool synMatch = (this->flags & TH_SYN) == tcp->isSyn();
    bool ackMatch = (this->flags & TH_ACK) == tcp->isAck();
    bool finMatch = (this->flags & TH_FIN) == tcp->isFin();
    bool flagsMatch = synMatch || ackMatch || finMatch;
    return (sourcePortMatch && destPortMatch && windowSizeMatch && flagsMatch);
}
