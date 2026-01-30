#include "../headers/PacketblockerGateway.h"
#include <memory>
#include <nlohmann/json_fwd.hpp>

void PacketblockerGateway::sendSelectedRules(std::shared_ptr<FilterRule> rule_) {
    std::vector<std::shared_ptr<FilterRule>> rules;
    auto logger = FilterRuleLogger::getInstance();
    if(rule_->getID() == -1) {
        rules = logger.findRulesByProperties(rule_);
    } else {
        rules.push_back(logger.findRulesByID(rule_->getID()));
    }
    MQConnector connector(this->mqPath);
    connector.connect();
    nlohmann::json jsonArr = nlohmann::json::array();
    for(auto rule : rules) {
        jsonArr.push_back(nlohmann::json::parse(rule->serializeToJSON()));
    }
    connector.sendData(jsonArr.dump());

    connector.sendData("end");
    connector.close();
}