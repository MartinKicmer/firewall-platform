#pragma once
#include <string>
#include <tuple>

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
};