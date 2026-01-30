#pragma  once
#include "FilterRule.h"
#include <memory>
#include <string>
#include "FilterRuleLogger.h"
#include "MQConnector.h"
class PacketblockerGateway {
public:
    PacketblockerGateway(const std::string mqPath_) 
    : mqPath(mqPath_) {}

    void sendSelectedRules(std::shared_ptr<FilterRule> rule);
private:
    std::string mqPath;
};