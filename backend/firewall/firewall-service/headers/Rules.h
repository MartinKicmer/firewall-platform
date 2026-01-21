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
    std::string source;
    std::string dest;
    int ttlMax;
    int ttlMin;

    L3Rule(bool permit_,int limitCount_,const std::string& source_, const std::string& dest_,int ttlMax_, int ttlMin_) :
    Rule(permit_,limitCount_),source(source_),dest(dest_),ttlMax(ttlMax_),ttlMin(ttlMin_) {}

    friend std::ostream& operator<<(std::ostream& o, const L3Rule& rule) {
        std::cout << "L2 RULE\n---------\n";
        std::cout << "ACTION: " << (rule.permit ? "permit" : "deny") << "\n";
        std::cout << "SOURCE MAC: " << rule.source << "\n";
        std::cout << "DESTINATION MAC: " << rule.dest << "\n";
        std::cout << "LIMIT COUNT: " << rule.limitCount << "\n";
        std::cout << "TTL MAX: " << rule.ttlMax << "\n";
        std::cout << "TTL MIN: " << rule.ttlMin << "\n";
        return o;
    }


};