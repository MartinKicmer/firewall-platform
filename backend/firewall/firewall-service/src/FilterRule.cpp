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


void FilterRule::formatL4TCPToJSON(nlohmann::json& j,std::shared_ptr<L4TcpRule> l4tcp) {
     j["ruleType"] = "L4TCP";
     j["data"] = {
            {"permit",l4tcp->permit},
            {"limitCount",l4tcp->limitCount},
            {"sourcePort",l4tcp->sPort},
            {"destPort",l4tcp->dPort},
            {"tcpFlags",l4tcp->flags},
            {"minWindowsize",l4tcp->minWindow},
            {"maxWindowsize",l4tcp->maxWindow}
    };
}


std::shared_ptr<L4TcpRule> FilterRule::deserializeL4TCPRule(const nlohmann::json& j) {
     std::shared_ptr<L4TcpRule> l4tcp = std::make_shared<L4TcpRule>(
        j["data"]["permit"],
        j["data"]["limitCount"],
        j["data"]["sourcePort"],
        j["data"]["destPort"],
        j["data"]["tcpFlags"],
        j["data"]["minWindowsize"],
        j["data"]["maxWindowsize"]
    );
    return l4tcp;
}

std::string FilterRule::serializeToJSON()  {
    nlohmann::json j;
    if (auto lredirect = std::dynamic_pointer_cast<RedirectRule>(this->rule)) {
        j["ruleType"] = lredirect->layer;
        j["redirect"] = {
            {"permit", lredirect->permit},
            {"count", lredirect->count},           
        };
        return j.dump();
    }
    j["ID"] = this->ID;
    j["ignore"] = this->ignore;
    if(auto selectRule = std::dynamic_pointer_cast<SelectRule>(this->rule)) {
        j["select"] = {
            {"permit",selectRule->permit},
            {"layer",selectRule->layer},
            {"fromMemory",selectRule->fromMemory}
        };
        return j.dump();
    }
    if(auto selectRule = std::dynamic_pointer_cast<RemoveRule>(this->rule)) {
        j["remove"] = {
            {"layer",selectRule->layer},
            {"fromMemory",selectRule->fromMemory}
        };
        return j.dump();
    }
    j["save"] = this->save;
    j["update"] = this->rule->update;
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
    if(auto l4Simple = std::dynamic_pointer_cast<L4SimpleRule>(this->rule)) {
        j["ruleType"] = "L4Simple";
        j["data"] = {
            {"permit", l4Simple->permit},
            {"limitCount", l4Simple->limitCount},
            {"sourcePort", l4Simple->sPort},
            {"destPort", l4Simple->dPort}
        };
    }
    if(auto l4tcp = std::dynamic_pointer_cast<L4TcpRule>(this->rule)) {
        this->formatL4TCPToJSON(j, l4tcp);
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
        {"maxTTL",l3rule->ttlMax},
        {"protocol",l3rule->protocol},
        {"TOS",l3rule->tos},
        {"allowFrag",l3rule->allowFragments}
    };
}

std::shared_ptr<FilterRule> FilterRule::deserialize(const std::string &jsonData) {
    auto j = nlohmann::json::parse(jsonData);
    std::shared_ptr<Rule> rule;
    if(j.contains("redirect")) {
        rule = std::make_shared<RedirectRule>(j["redirect"]["permit"],j["ruleType"],j["redirect"]["count"]);
        return std::make_shared<FilterRule>(rule, -1);
    }
    int id = j["ID"];
    bool ignore = j["ignore"];
    if(j.contains("select")) {
        bool permit = j["select"]["permit"];
        std::string layer(j["select"]["layer"]);
        bool fromMemory = j["select"]["fromMemory"];
        rule = std::make_shared<SelectRule>(permit,id,layer,fromMemory);
        return std::make_shared<FilterRule>(rule, id);
    }
    if(j.contains("remove")) {
        std::string layer(j["remove"]["layer"]);
        bool fromMemory = j["remove"]["fromMemory"];
        rule = std::make_shared<RemoveRule>(id,fromMemory,layer);
        return std::make_shared<FilterRule>(rule, id);
    }

    bool save = false;
    if(j.contains("save")) {
        save = j["save"];
    }

    if (j["ruleType"] == "L2") {
        rule = std::make_shared<L2Rule>(
            j["data"]["permit"],
            j["data"]["limitCount"],
            j["data"]["source"],
            j["data"]["dest"],
            j["update"]
        );
    }

    if(j["ruleType"] == "L3") {
        rule = deserializeL3Rule(j);
    }

    if(j["ruleType"] == "L4Simple") {
        rule = std::make_shared<L4SimpleRule>(j["data"]["permit"],
            j["data"]["limitCount"],
            j["data"]["sourcePort"],
            j["data"]["destPort"],j["update"]);
    }
    if(j["ruleType"] == "L4TCP") {
        rule = deserializeL4TCPRule(j);
    }
    return std::make_shared<FilterRule>(rule, id,save,ignore);
}

std::shared_ptr<L3Rule> FilterRule::deserializeL3Rule(const nlohmann::json& j) {
    std::shared_ptr<L3Rule> l3rule = std::make_shared<L3Rule>(
        j["data"]["permit"],
        j["data"]["limitCount"],
        j["data"]["source"],
        j["data"]["dest"],
        j["data"]["maxTTL"],
        j["data"]["minTTL"],
        j["data"]["protocol"],
        j["data"]["TOS"],
        j["data"]["allowFrag"],
        j["update"]
    );
    return l3rule;
}


void FilterRule::printRule() {
    std::cout << "FILTER RULE: \n------" << std::endl;
    std::cout << "ID: " << this->ID << std::endl;

    if (auto l4TCP = std::dynamic_pointer_cast<L4TcpRule>(this->rule)) {
        std::cout << *l4TCP << std::endl;
    } 
    else if (auto l4Simple = std::dynamic_pointer_cast<L4SimpleRule>(this->rule)) {
        std::cout << *l4Simple << std::endl;
    }
    else if (auto l3rule = std::dynamic_pointer_cast<L3Rule>(this->rule)) {
        std::cout << *l3rule << std::endl;
    }
    else if (auto l2rule = std::dynamic_pointer_cast<L2Rule>(this->rule)) {
        std::cout << *l2rule << std::endl;
    }
}