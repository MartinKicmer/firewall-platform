#include "../headers/FilterRule.h"
#include <memory>
#include <nlohmann/json_fwd.hpp>

bool FilterRule::canPass() {
    if(this->ruleComparer) {
        return this->ruleComparer->compare(this->rule);
    } else {
        throw std::runtime_error("Comparer wasnt set for Filter rule\n");
    }
}

std::string FilterRule::serializeToJSON()  {
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
    if(auto l3 = std::dynamic_pointer_cast<L3Rule>(this->rule)) {
        this->formatL3ToJSON(j,l3);
    }
    return j.dump();
}

void FilterRule::formatL3ToJSON(nlohmann::json& j,std::shared_ptr<L3Rule> l3rule) {
    j["ruleType"] = "L3";
    j["data"] = {
        {"permit",l3rule->permit},
        {"limitCount",l3rule->limitCount},
        {"source",l3rule->source},
        {"dest",l3rule->dest},
        {"minTTL",l3rule->ttlMin},
        {"maxTTL",l3rule->ttlMax}
    };
}

std::shared_ptr<FilterRule> FilterRule::deserialize(const std::string &jsonData) {
     auto j = nlohmann::json::parse(jsonData);
        int id = j["ID"];
        std::shared_ptr<Rule> rule;
        if (j["ruleType"] == "L2") {
            rule = std::make_shared<L2Rule>(
                j["data"]["permit"],
                j["data"]["limitCount"],
                j["data"]["source"],
                j["data"]["dest"]
            );
        }

        if(j["ruleType"] == "L3") {
            rule = deserializeL3Rule(j);
        }
    return std::make_shared<FilterRule>(rule, id);
}

std::shared_ptr<L3Rule> FilterRule::deserializeL3Rule(const nlohmann::json& j) {
    std::shared_ptr<L3Rule> l3rule = std::make_shared<L3Rule>(
        j["data"]["permit"],
        j["data"]["limitCount"],
        j["data"]["source"],
        j["data"]["dest"],
        j["data"]["maxTTL"],
        j["data"]["minTTL"]
    );
    return l3rule;
}