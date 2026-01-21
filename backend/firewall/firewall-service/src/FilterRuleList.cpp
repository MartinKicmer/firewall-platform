#include "../headers/FilterRuleList.h"

void FilterRuleList::addRule(std::shared_ptr<FilterRule> filterRule) {
    this->filterRules[filterRule->getID()] = filterRule;
    std::cout << "Rule added\n";
}

std::shared_ptr<FilterRule> FilterRuleList::checkAllRules() {
    for (auto& [ID,rule] : this->filterRules) {
        if(!this->packetParser) throw std::runtime_error("Filter list doesnt have parser\n");
        rule->setParser(this->packetParser);
        if(!rule->canPass()) {
            return rule;
        }
    }
    return nullptr;
}

void FilterRuleList::printFilterRuleInfo(std::shared_ptr<FilterRule> rule) {
    if(auto derived = std::dynamic_pointer_cast<L2Rule>(rule->getRule())) {
        std::cout << "L2 Layer rule\n------------------\n";
        std::cout << "Source MAC: " << derived->source << "\n";
        std::cout << "Destination MAC: " << derived->dest << "\n";
    } 
}