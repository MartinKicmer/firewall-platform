#pragma  once
#include "FilterRule.h"
#include <memory>
#include <string>
#include "FilterRuleLogger.h"
#include "MQConnector.h"
#include "FilterRuleList.h"
class PacketblockerGateway {
public:
    PacketblockerGateway(const std::string mqPath_,std::shared_ptr<FilterRuleList> list_) 
    : mqPath(mqPath_),list(list_) {}

    void sendSelectedRules(std::shared_ptr<FilterRule> rule);
    void sendFromMemory();
private:
    std::string mqPath;
    std::shared_ptr<FilterRuleList> list;
};