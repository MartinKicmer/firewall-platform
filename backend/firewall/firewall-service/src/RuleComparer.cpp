#include "../headers/RuleComparer.h"

bool RuleComparer::compare(std::shared_ptr<Rule> rule) const {
    if(auto l2rule = std::dynamic_pointer_cast<L2Rule>(rule)) {
        auto frame = this->parser->getEthernetFrame();
        const auto& [source,dest] = frame->getMACAddresses();
        bool sourceMatch = (l2rule->source == "none" || source == l2rule->source);
        bool destMatch   = (l2rule->dest == "none" || dest == l2rule->dest);
        if (sourceMatch && destMatch) {
            return l2rule->permit; 
        }
    }
    if(auto l3rule = std::dynamic_pointer_cast<L3Rule>(rule)) {
        auto datagram = this->parser->getIPv4Datagram();

    }
    return true;
}