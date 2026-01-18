#pragma once
#include "Rules.h"
#include <memory>
#include <array>
#include <tuple>
#include <cstdlib>
#include "PacketParser.h"
#include <nlohmann/json.hpp>

class FilterRule {
public:
    FilterRule(std::shared_ptr<Rule> rule = nullptr,int ID = -1) 
    : rule(rule),ID(ID) {}
    bool canPass(std::tuple<ssize_t, std::array<uint8_t, BUFSIZ>>& data);
    void setParser(std::shared_ptr<PacketParser> parser) { this->packetParser = parser; }
    std::shared_ptr<Rule> getRule() { return this->rule; }
    [[nodiscard]] int getID() const { return this->ID; } 

    [[nodiscard]] std::string serializeToJSON() const;

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
    int ID;
    std::shared_ptr<Rule> rule;
    std::shared_ptr<PacketParser> packetParser;
};