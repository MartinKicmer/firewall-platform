#pragma once
#include "FilterRule.h"
#include <unordered_map>
class FilterRuleList {

public:
    FilterRuleList(std::shared_ptr<PacketParser> packetParser = nullptr) 
    : packetParser(packetParser) {}
    void addRule(int ID,std::shared_ptr<Rule> rule);
    void setParser(std::shared_ptr<PacketParser> packetParser) { this->packetParser = packetParser; }
    std::shared_ptr<FilterRule> checkAllRules(std::tuple<ssize_t, std::array<uint8_t, BUFSIZ>>& data);
    void printFilterRuleInfo(std::shared_ptr<FilterRule> rule);
private:
    std::unordered_map<int,std::shared_ptr<FilterRule>> filterRules;
    std::shared_ptr<PacketParser> packetParser;

};