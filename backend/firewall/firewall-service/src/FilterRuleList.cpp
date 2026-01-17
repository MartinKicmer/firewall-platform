#include "../headers/FilterRuleList.h"

void FilterRuleList::addRule(int ID,std::shared_ptr<Rule> rule) {
    std::shared_ptr<FilterRule> filterRule = std::make_shared<FilterRule>(rule,ID);
    this->filterRules[ID] = filterRule;
}

std::shared_ptr<FilterRule> FilterRuleList::checkAllRules(std::tuple<ssize_t, std::array<uint8_t, BUFSIZ>>& data) {
    for (auto& [ID,rule] : this->filterRules) {
        if(!this->packetParser) throw std::runtime_error("Filter list doesnt have parser\n");
        rule->setParser(this->packetParser);
        if(!rule->canPass(data)) {
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