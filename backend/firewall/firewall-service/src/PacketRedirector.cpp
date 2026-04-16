#include "../headers/PacketRedirector.h"
#include <memory>
#include <optional>
void PacketRedirector::redirectPacket(std::shared_ptr<PacketParser> parser) {
    if(!this->canRedirect()) return;

    MQConnector connector("/firewallRedirect");
    connector.connect();

    auto redirectRule = std::dynamic_pointer_cast<RedirectRule>(this->rule->getRule());
    std::cout << "LAYER : " << redirectRule->layer  << "\n\n\n\n\n\n\n\n"<< std::endl;
    if(redirectRule->layer == "L2") {
        auto frame = parser->getEthernetFrame();
        if(frame) {
            connector.sendData(frame->serialize().dump());
        }
    }
    if(redirectRule->layer == "L3") {
        auto iPv4Datagram = parser->getIPv4Datagram();
        if(iPv4Datagram) {
            connector.sendData(iPv4Datagram->serialize().dump());
        }
    }
    if(redirectRule->layer == "L4Simple") {
        auto udp = parser->getUDPDatagram();
        if(udp) {
            connector.sendData(udp->serialize().dump());
        }
    }
    if(redirectRule->layer == "L4TCP") {
        auto tcp = parser->getTCPPACKET();
        if(tcp) {
            connector.sendData(tcp->serialize().dump());
        }
    }
    if(redirectRule->count != -1) redirectRule->count--;

    if(redirectRule->count == 0) {
        connector.sendData("end");
        this->redirect = false;
        this->rule = nullptr;
    }
    
    connector.close(); 
}

 std::optional<nlohmann::json> PacketRedirector::recievePacket() {
    if(this->canRedirect()) {
        MQConnector connector("/firewallRedirect");
        connector.create();
        const auto& [end,data] = connector.recieveData();
        if(end) {
            connector.close();
            return std::nullopt;
        } else {
            auto json = nlohmann::json::parse(data);
            return std::make_optional(json);
        }
    }
    return nullptr;
}