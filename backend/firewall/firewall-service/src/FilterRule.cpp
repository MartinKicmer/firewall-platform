#include "../headers/FilterRule.h"

bool FilterRule::canPass() {
    if(auto l2rule = std::dynamic_pointer_cast<L2Rule>(this->rule)) {
        auto frame = this->packetParser->parseEthernetFrame();
        const auto& [source,dest] = frame->getMACAddresses();
        bool sourceMatch = (l2rule->source == "none" || source == l2rule->source);
        bool destMatch   = (l2rule->dest == "none" || dest == l2rule->dest);
        if (sourceMatch && destMatch) {
            return l2rule->permit; 
        }
    }
    if(auto l3rule = std::dynamic_pointer_cast<L2Rule>(this->rule)) {
        
    }
    
    return true;
}

std::string FilterRule::serializeToJSON() const {
    nlohmann::json j;
    j["ID"] = this->ID;
    if (auto l2 = std::dynamic_pointer_cast<L2Rule>(this->rule)) {
        j["ruleType"] = "L2";
        j["data"] = {
            {"permit", l2->permit},
            {"limitCount", l2->limitCount},
            {"source", l2->source},
            {"dest", l2->dest}
        };
    }
    return j.dump();
}