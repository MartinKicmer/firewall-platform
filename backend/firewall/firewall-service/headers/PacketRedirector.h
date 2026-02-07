#pragma once
#include <future>
#include <nlohmann/json.hpp>
#include <string>
#include "AbstractPDU.h"
#include "FilterRule.h"
#include "PacketParser.h"
#include <memory>
#include "MQConnector.h"
#include "Rules.h"
#include "EthernetFrame.h"
#include <optional>


class PacketRedirector {
public:
    PacketRedirector() : redirect(false) {}
     
    void redirectPacket(std::shared_ptr<PacketParser> parser);
    std::optional<nlohmann::json> recievePacket();
    bool canRedirect() const { return this->redirect; }
    void setRule(std::shared_ptr<FilterRule> rule_) {
        if(this->redirect) return; 
        this->rule = rule_; 
        this->redirect = true;
    }
    std::shared_ptr<FilterRule> getRule() { return this->rule; }
private: 
    bool redirect;
    std::shared_ptr<FilterRule> rule;
};