#include "../headers/FilterRule.h"

bool FilterRule::canPass(std::tuple<ssize_t, std::array<uint8_t, BUFSIZ>>& data) {
    bool matched = false;
    if(auto l2rule = std::dynamic_pointer_cast<L2Rule>(this->rule)) {
        auto frame = this->packetParser->parseEthernetFrame();
        const auto& [source,dest] = frame->getMACAddresses();
        bool sourceMatch = (l2rule->source == "none" || source == l2rule->source);
        bool destMatch   = (l2rule->dest == "none" || dest == l2rule->dest);
        if (sourceMatch && destMatch) {
            return l2rule->permit; 
        }
    }

    return true;
}