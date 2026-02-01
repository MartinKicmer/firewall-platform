#pragma once
#include "Rules.h"
#include <memory>
#include <array>
#include <tuple>
#include <cstdlib>
#include "PacketParser.h"
#include <nlohmann/json.hpp>
#include "RuleComparer.h"
class FilterRule {
public:
    FilterRule(std::shared_ptr<Rule> rule_ = nullptr,int ID_ = -1) 
    : rule(rule_),ID(ID_),save(false) {}
    FilterRule(std::shared_ptr<Rule> rule_ ,int ID_ ,bool save_) 
    : rule(rule_),ID(ID_),save(save_) {}
    bool canPass();
    bool canSave() const { return this->save; }
    bool canUpdate() const { return this->rule->update; }
    void changeSave(bool s) { this->save = s;  }
    void setComparer(std::shared_ptr<RuleComparer> comparer_) { this->ruleComparer = comparer_; }
    std::shared_ptr<Rule> getRule() { return this->rule; }
    [[nodiscard]] int getID() const { return this->ID; } 

    [[nodiscard]] std::string serializeToJSON();

    static std::shared_ptr<FilterRule> deserialize(const std::string& jsonData);
private:
    std::shared_ptr<Rule> rule;
    int ID;
    bool save;
    std::shared_ptr<RuleComparer> ruleComparer;

    void formatL3ToJSON(nlohmann::json& j,std::shared_ptr<L3Rule> l3rule);
    static std::shared_ptr<L3Rule> deserializeL3Rule(const nlohmann::json& j);
};