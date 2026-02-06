#pragma once
#include "AbstractPDU.h"
#include "Rules.h"
#include "PacketParser.h"
#include <memory>
#include <tuple>
#include <bitset>
#include <sstream>
#include <string>
class Rule;
class RuleComparer {
public:
    RuleComparer(std::shared_ptr<PacketParser> parser_) : parser(parser_) {}
    [[nodiscard]] bool compare(std::shared_ptr<Rule> rule) const;

     static bool matchIP(const std::string& targetIP, const std::tuple<std::string,int>& ipWithPrefix);
private:
    std::shared_ptr<PacketParser> parser;
};