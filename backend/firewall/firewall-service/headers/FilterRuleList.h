#pragma once
#include "FilterRule.h"
#include <unordered_map>
#include "RuleComparer.h"
#include <shared_mutex>
#include <mutex>         
class FilterRuleList {
public:
    FilterRuleList(std::shared_ptr<PacketParser> packetParser_ = nullptr) 
    : packetParser(packetParser_),ruleComparer(nullptr) {}
    void addRule(std::shared_ptr<FilterRule> filterRule);
    void removeRule(std::shared_ptr<FilterRule> filterRule);
    void setParser(std::shared_ptr<PacketParser> packetParser_) { 
        if(!packetParser_) throw std::runtime_error("Trying to set invalid parser\n");
        this->packetParser = packetParser_; 
        this->ruleComparer = std::make_shared<RuleComparer>(this->packetParser);
    }
    std::shared_ptr<FilterRule> checkAllRules();
    void printFilterRuleInfo(std::shared_ptr<FilterRule> rule);

    const std::unordered_map<int,std::shared_ptr<FilterRule>>& getRules() { return this->filterRules; }
private:
    std::unordered_map<int,std::shared_ptr<FilterRule>> filterRules;
    std::shared_ptr<PacketParser> packetParser;
    std::shared_ptr<RuleComparer> ruleComparer;
    mutable std::shared_mutex rulesMtx;

};