#pragma once
#include <string>
#include <tuple>
#include <iostream>

struct Rule {
    bool permit;
    int limitCount;
    virtual ~Rule() = default;
    Rule() : permit(true),limitCount(-1) {}
    Rule(bool permit_,bool limitCount_) : permit(permit_),limitCount(limitCount_) {}
};

struct L2Rule : public Rule {
    std::string source;
    std::string dest;
    L2Rule(bool permit_,int limitCount_,const std::string& source_,const std::string& dest_) : Rule(permit_,limitCount_), source(source_),dest(dest_) {}

    friend std::ostream& operator<<(std::ostream& o, const L2Rule& rule) {
        std::cout << "L2 RULE\n---------\n";
        std::cout << "ACTION: " << (rule.permit ? "permit" : "deny") << "\n";
        std::cout << "SOURCE MAC: " << rule.source << "\n";
        std::cout << "DESTINATION MAC: " << rule.dest << "\n";
        std::cout << "LIMIT COUNT: " << rule.limitCount << "\n";
        return o;
    }

};

struct L3Rule : public Rule {
    std::tuple<std::string,int> source;
    std::tuple<std::string,int> dest;
    int ttlMax;
    int ttlMin;

    int protocol;
    int tos;
    bool allowFragments;

    L3Rule(bool permit_,
        int limitCount_,
        const std::tuple<std::string,int>& source_,
        const std::tuple<std::string,int>& dest_,
        int ttlMax_, 
        int ttlMin_,
        int protocol_ = -1,
        int tos_ = -1,
        bool allowFrags_ = true) :
    Rule(permit_, limitCount_), 
    source(source_), dest(dest_), 
    ttlMax(ttlMax_), ttlMin(ttlMin_), 
    protocol(protocol_), tos(tos_), 
    allowFragments(allowFrags_) {}

    friend std::ostream& operator<<(std::ostream& o, const L3Rule& rule) {
        const auto& [source,sourcePrefix] = rule.source;
        const auto& [dest,destPrefix] = rule.dest;
        std::cout << "L3 RULE\n---------\n";
        std::cout << "ACTION: " << (rule.permit ? "permit" : "deny") << "\n";
        std::cout << "SOURCE IP: " << source << " PREFIX: " << sourcePrefix << "\n";
        std::cout << "DESTINATION IP: " << dest << " PREFIX: " << destPrefix << "\n";
        std::cout << "LIMIT COUNT: " << rule.limitCount << "\n";
        std::cout << "TTL MAX: " << rule.ttlMax << "\n";
        std::cout << "TTL MIN: " << rule.ttlMin << "\n";
        std::cout << "PROTOCOL: " << rule.protocol << "\n";
        std::cout << "TOS: " << rule.tos << "\n";
        std::cout << "allowFrags_: " << rule.allowFragments << "\n";
        return o;
    }

};


struct RedirectRule : public Rule {
    RedirectRule(bool permit_,const std::string& layer_,int count_) 
    : Rule(permit_,-1),layer(layer_),count(count_) {}
    std::string layer;
    int count;
};