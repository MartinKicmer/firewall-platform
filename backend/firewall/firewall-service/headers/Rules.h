#pragma once
#include "PacketParser.h"
#include <cstdint>
#include <memory>
#include <string>
#include <tuple>
#include <iostream>
#include "RuleComparer.h"

struct Rule {
    bool update = false;
    bool permit;
    int limitCount = -1;
    virtual ~Rule() = default;
    virtual bool match(std::shared_ptr<PacketParser> parser) {
        return true;
    }

    Rule() : permit(true),limitCount(-1) {}
    Rule(bool permit_,bool limitCount_) : permit(permit_),limitCount(limitCount_) {}
    Rule(bool permit_,bool limitCount_,bool update_) : permit(permit_),limitCount(limitCount_),update(update_) {}
};

struct L2Rule : public Rule {
    std::string source;
    std::string dest;
    L2Rule(bool permit_,int limitCount_,const std::string& source_,const std::string& dest_) : Rule(permit_,limitCount_), source(source_),dest(dest_) {}
    L2Rule(bool permit_,int limitCount_,const std::string& source_,const std::string& dest_,bool update_) : Rule(permit_,limitCount_,update_), source(source_),dest(dest_) {}
    friend std::ostream& operator<<(std::ostream& o, const L2Rule& rule) {
        std::cout << "L2 RULE\n---------\n";
        std::cout << "ACTION: " << (rule.permit ? "permit" : "deny") << "\n";
        std::cout << "SOURCE MAC: " << rule.source << "\n";
        std::cout << "DESTINATION MAC: " << rule.dest << "\n";
        std::cout << "LIMIT COUNT: " << rule.limitCount << "\n";
        return o;
    }
    bool match(std::shared_ptr<PacketParser> parser) override;
};

struct L3Rule : public Rule {
    std::tuple<std::string,int> source;
    std::tuple<std::string,int> dest;
    int ttlMax = -1;
    int ttlMin = -1;

    int protocol = -1;
    int tos = -1;
    bool allowFragments = false;

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

    L3Rule(bool permit_,
        int limitCount_,
        const std::tuple<std::string,int>& source_,
        const std::tuple<std::string,int>& dest_,
        int ttlMax_, 
        int ttlMin_,
        int protocol_,
        int tos_,
        bool allowFrags_,bool update_) :
    Rule(permit_, limitCount_,update_), 
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


    bool match(std::shared_ptr<PacketParser> parser) override;

};


struct L4SimpleRule : public Rule {
    L4SimpleRule(bool permit_,
        int limitCount_,int sPort_,int dPort_) : Rule(permit_,limitCount_) , sPort(sPort_),dPort(dPort_) {}

    L4SimpleRule(bool permit_,
        int limitCount_,int sPort_,int dPort_,bool update_) : Rule(permit_,limitCount_,update_) , sPort(sPort_),dPort(dPort_) {}
    friend std::ostream& operator<<(std::ostream& o, const L4SimpleRule& rule) {
        std::cout << "L4Simple RULE\n---------\n";
        std::cout << "ACTION: " << (rule.permit ? "permit" : "deny") << "\n";
        std::cout << "LIMIT COUNT: " << rule.limitCount << "\n";
        std::cout << "SOURCE PORT: " << rule.sPort << "\n";
        std::cout << "DEST PORT: " << rule.dPort << "\n";
        return o;
    }
    bool match(std::shared_ptr<PacketParser> parser) override;
    int sPort;
    int dPort;
};

struct L4TcpRule : public L4SimpleRule {
    uint8_t flags;
    short minWindow;
    short maxWindow;

    L4TcpRule(bool permit_, int limitCount_, int sPort_, int dPort_, 
              uint8_t flags_ = 0, short minW = -1, short maxW = -1) 
        : L4SimpleRule(permit_, limitCount_, sPort_, dPort_), 
          flags(flags_), minWindow(minW), maxWindow(maxW) {}

    friend std::ostream& operator<<(std::ostream& o, const L4TcpRule& rule) {
        std::cout << "L4TCP RULE\n---------\n";
        std::cout << "ACTION: " << (rule.permit ? "permit" : "deny") << "\n";
        std::cout << "LIMIT COUNT: " << rule.limitCount << "\n";
        std::cout << "SOURCE PORT: " << rule.sPort << "\n";
        std::cout << "DEST PORT: " << rule.dPort << "\n";
        std::cout << "MAX WINDOW SIZE: " << rule.maxWindow << "\n";
        std::cout << "MIN WINDOW SIZET: " << rule.minWindow << "\n";
        std::cout << "FLAGS: " << static_cast<int>(rule.flags) << "\n";

        return o;
    }
    bool match(std::shared_ptr<PacketParser> parser) override;
};
struct RemoveRule : public Rule {
    RemoveRule(int ID_,bool fromMemory_,const std::string& layer_) 
    : Rule(false, -1),ID(ID_),fromMemory(fromMemory_),layer(layer_) {} 
    int ID;
    bool fromMemory;
    std::string layer;
};

struct SelectRule : public Rule {
    SelectRule(bool permit_,int ID_,const std::string& layer_,bool fromMemory_) 
    : Rule(permit_,-1),ID(ID_),layer(layer_ ),fromMemory(fromMemory_) {}

    int ID;
    std::string layer;
    bool fromMemory;
};
struct RedirectRule : public Rule {
    RedirectRule(bool permit_,const std::string& layer_,int count_) 
    : Rule(permit_,-1),layer(layer_),count(count_) {}
    std::string layer;
    int count;
};