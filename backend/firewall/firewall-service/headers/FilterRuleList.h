#pragma once
#include "FilterRule.h"
#include <unordered_map>
class FilterRuleList {

public:
    FilterRuleList(std::shared_ptr<PacketParser> packetParser_ = nullptr) 
    : packetParser(packetParser_) {}
    void addRule(std::shared_ptr<FilterRule> filterRule);
    void setParser(std::shared_ptr<PacketParser> packetParser_) { this->packetParser = packetParser_; }
    std::shared_ptr<FilterRule> checkAllRules();
    void printFilterRuleInfo(std::shared_ptr<FilterRule> rule);
private:
    std::unordered_map<int,std::shared_ptr<FilterRule>> filterRules;
    std::shared_ptr<PacketParser> packetParser;

};