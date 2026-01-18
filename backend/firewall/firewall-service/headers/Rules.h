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
    L2Rule(bool permit_,bool limitCount_,const std::string& source,const std::string& dest) : Rule(permit_,limitCount_), source(source),dest(dest) {}

    friend std::ostream& operator<<(std::ostream& o, const L2Rule& rule) {
        std::cout << "L2 RULE\n---------\n";
        std::cout << "ACTION: " << (rule.permit ? "permit" : "deny") << "\n";
        std::cout << "SOURCE MAC: " << rule.source << "\n";
        std::cout << "DESTINATION MAC: " << rule.dest << "\n";
        std::cout << "LIMIT COUNT: " << rule.limitCount << "\n";
        return o;
    }

};