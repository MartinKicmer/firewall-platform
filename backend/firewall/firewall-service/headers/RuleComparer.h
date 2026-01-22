#pragma once
#include "AbstractPDU.h"
#include "Rules.h"
#include "PacketParser.h"
#include <memory>
class RuleComparer {
public:
    RuleComparer(std::shared_ptr<PacketParser> parser_) : parser(parser_) {}
    [[nodiscard]] bool compare(std::shared_ptr<Rule> rule) const;
private:
    std::shared_ptr<PacketParser> parser;


};