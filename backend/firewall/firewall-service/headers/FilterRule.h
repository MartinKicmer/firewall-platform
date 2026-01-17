#pragma once
#include "Rules.h"
#include <memory>
#include <array>
#include <tuple>
#include <cstdlib>
#include "PacketParser.h"


class FilterRule {
public:
    FilterRule(std::shared_ptr<Rule> rule = nullptr,int ID = -1) 
    : rule(rule),ID(ID) {}
    bool canPass(std::tuple<ssize_t, std::array<uint8_t, BUFSIZ>>& data);
    void setParser(std::shared_ptr<PacketParser> parser) { this->packetParser = parser; }
    std::shared_ptr<Rule> getRule() { return this->rule; }
    [[nodiscard]] int getID() const { return this->ID; } 
private:
    int ID;
    std::shared_ptr<Rule> rule;
    std::shared_ptr<PacketParser> packetParser;
};