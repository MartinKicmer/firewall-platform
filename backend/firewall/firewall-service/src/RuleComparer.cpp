#include "../headers/RuleComparer.h"

bool RuleComparer::matchIP(const std::string& targetIP, const std::tuple<std::string,int>& ipWithPrefix) {
    const auto& [ruleIP, prefix] = ipWithPrefix;

    uint32_t targetAddr, ruleAddr;

    inet_pton(AF_INET, targetIP.c_str(), &targetAddr);
    inet_pton(AF_INET, ruleIP.c_str(), &ruleAddr);
    targetAddr = ntohl(targetAddr);
    ruleAddr = ntohl(ruleAddr);

    uint32_t mask = (prefix == 0) ? 0 : (~0U << (32 - prefix));

    return (targetAddr & mask) == (ruleAddr & mask);
}

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