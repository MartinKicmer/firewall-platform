#include "../headers/RuleComparer.h"
#include <netinet/tcp.h>

bool RuleComparer::matchIP(const std::string& targetIP, const std::tuple<std::string,int>& ipWithPrefix) {
    const auto& [ruleIP, prefix] = ipWithPrefix;

    if(ruleIP == "none" || ruleIP == "any" || prefix == -1) return true;

    uint32_t targetAddr, ruleAddr;

    if (inet_pton(AF_INET, targetIP.c_str(), &targetAddr) != 1) return false;
    if (inet_pton(AF_INET, ruleIP.c_str(), &ruleAddr) != 1) return false;
    if (prefix == 32) return (targetAddr == ruleAddr);
    if (prefix == 0) return true;

    uint32_t mask = (prefix == 32) ? 0xFFFFFFFF : htonl(~((1U << (32 - prefix)) - 1));

    return (targetAddr & mask) == (ruleAddr & mask);
}

bool RuleComparer::compare(std::shared_ptr<Rule> rule) const {
    if(rule->match(this->parser)) {
        return rule->permit;
    }
    return true;
}