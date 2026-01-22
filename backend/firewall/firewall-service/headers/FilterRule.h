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
    : rule(rule_),ID(ID_) {}
    bool canPass();
    void setComparer(std::shared_ptr<RuleComparer> comparer_) { this->ruleComparer = comparer_; }
    std::shared_ptr<Rule> getRule() { return this->rule; }
    [[nodiscard]] int getID() const { return this->ID; } 

    [[nodiscard]] std::string serializeToJSON();

    static std::shared_ptr<FilterRule> deserialize(const std::string& jsonData) {
        auto j = nlohmann::json::parse(jsonData);
        int id = j["ID"];
        std::shared_ptr<Rule> rule;

        if (j["ruleType"] == "L2") {
            rule = std::make_shared<L2Rule>(
                j["data"]["permit"],
                j["data"]["limitCount"],
                j["data"]["source"],
                j["data"]["dest"]
            );
        }
        return std::make_shared<FilterRule>(rule, id);
    }
private:
    std::shared_ptr<Rule> rule;
    int ID;
    std::shared_ptr<RuleComparer> ruleComparer;

    void formatL3ToJSON(nlohmann::json& j,std::shared_ptr<L3Rule> l3rule);
};