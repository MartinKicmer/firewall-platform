#include "../headers/PacketRedirector.h"
#include <cstddef>
#include <memory>
#include <optional>
void PacketRedirector::redirectPacket(std::shared_ptr<PacketParser> parser) {
    if(!this->canRedirect()) return;

    MQConnector connector("/firewallRedirect");
    connector.connect();

    auto redirectRule = std::dynamic_pointer_cast<RedirectRule>(this->rule->getRule());
    if(redirectRule->layer == "L2") {
        auto frame = parser->getEthernetFrame();
        if(frame) {
            connector.sendData(frame->serialize().dump());
            redirectRule->count--;
        }
    }
    if(redirectRule->count <= 0) {
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