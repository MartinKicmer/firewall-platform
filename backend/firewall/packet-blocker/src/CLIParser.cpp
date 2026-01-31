#include "../headers/CLIParser.h"
#include <cstdlib>
#include <cstring>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>

bool CLIParser::contains(const std::string& arg) {
    for(int i = 0; i < this->argc; ++i) {
        if(!std::strcmp(this->argv[i],arg.c_str())) {
            return true;
        }
    }
    return false;
}


std::shared_ptr<SelectRule> CLIParser::parseSelectRule() {
    std::string layer = this->parseLayer();
    int ID = this->parseRID();
    bool permit = this->parseAction();
    bool fromMemory = false;
    for(int i = 0; i < this->argc - 1; ++i) {
        if(!std::strcmp(this->argv[i],"-memory")) {
            fromMemory = (!std::strcmp(this->argv[i+1],"true"));
        }
    }
    return std::make_shared<SelectRule>(permit,ID,layer,fromMemory);
}


int CLIParser::parseRID() {
    if(this->argc < 3) throw std::runtime_error("Invalid CLI number of args\n");
    for (int i = 0; i < this->argc - 1; ++i) {
        if (!std::strcmp(this->argv[i], "-rid")) {
            return std::stoi(this->argv[i + 1]);
        }
    }
    return -1;
}
std::shared_ptr<L2Rule> CLIParser::parseL2Rule() {
    if (this->argc < 5) {
        throw std::runtime_error("Invalid CLI format (too few arguments)\n");
    }

    std::string smac = "none";
    std::string dmac = "none";
    for (int i = 0; i < this->argc - 1; ++i) {
        if (std::strcmp(this->argv[i], "-smac") == 0) {
            smac = this->argv[i + 1];
        } else if (std::strcmp(this->argv[i], "-dmac") == 0) {
            dmac = this->argv[i + 1];
        } 
    }

    bool permit = this->parseAction();
    return std::make_shared<L2Rule>(permit, -1, smac, dmac);
}

std::tuple<int,int,bool> CLIParser::parseIPINFO() {
    int TOS = -1;
    int protocol = -1;
    bool allowFrags = true;

    for(int i = 0; i < this->argc - 1; ++i) {
        if(!std::strcmp(this->argv[i],"-proto")) {
            protocol = this->protocols.at(this->argv[i+1]);
        }
        if(!std::strcmp(this->argv[i],"-allowFrag")) {
            allowFrags = (std::strcmp(this->argv[i+1],"permit") == 0);
        }
        if(!std::strcmp(this->argv[i],"-tos")) {
            TOS = std::atoi(this->argv[i+1]);
        }
    }

    return std::make_tuple(protocol,TOS,allowFrags);
}

std::shared_ptr<RedirectRule> CLIParser::parseRedirectRule() {
    bool permit = this->parseAction();
    int count = -1;
    std::string layer = this->parseLayer();
    for(int i = 0; i < this->argc - 1; ++i) {
        if(!std::strcmp(this->argv[i],"redirect")) {
            count = std::atoi(this->argv[i+1]);
        }
    }
    return std::make_shared<RedirectRule>(permit,layer,count);
}


std::shared_ptr<RemoveRule> CLIParser::parseRemoveRule() {
    std::string layer = this->parseLayer();
    int ID = this->parseRID();
    bool fromMemory = false;
    for(int i = 0; i < this->argc - 1; ++i) {
        if(!std::strcmp(this->argv[i],"-memory")) {
            fromMemory = (!std::strcmp(this->argv[i+1],"true"));
        }
    }
    return std::make_shared<RemoveRule>(ID,fromMemory,layer);
}

std::shared_ptr<L4SimpleRule> CLIParser::parseSimpleL4Rule() {
    std::string layer = this->parseLayer();
    int limitCount = -1;
    bool permit = this->parseAction();
    int sPort = -1;
    int dPort = -1;

    for(int i = 0; i < this->argc - 1; ++i) {
        if(!std::strcmp(this->argv[i],"-sport")) sPort = std::atoi(this->argv[i+1]);
        if(!std::strcmp(this->argv[i],"-dport")) dPort = std::atoi(this->argv[i+1]);
    }

    return std::make_shared<L4SimpleRule>(permit,limitCount,sPort,dPort);
}
std::shared_ptr<FilterRule> CLIParser::parseCLIArguments() {
    if(this->contains("redirect")) {
        auto rule = this->parseRedirectRule();        
        return std::make_shared<FilterRule>(rule,-1);
    }
    if(this->contains("select")) {
        auto rule = this->parseSelectRule();
        return std::make_shared<FilterRule>(rule,this->parseRID());
    }
    if(this->contains("remove")) {
        auto rule = this->parseRemoveRule();
        return std::make_shared<FilterRule>(rule,this->parseRID());
    }
    std::shared_ptr<FilterRule> filterRule;
    int RID = this->parseRID();
    if(RID == -1) {
        throw std::runtime_error("Could not find RID\n");
    }
    std::string layer = this->parseLayer();
    bool save = this->contains("-save");
    if(layer == "L2") {
        auto rule = this->parseL2Rule();
        filterRule = std::make_shared<FilterRule>(rule,RID,save);
        return filterRule;
    }
    if(layer == "L3") {
        auto rule = this->parseL3Rule();
        filterRule = std::make_shared<FilterRule>(rule,RID,save);
        return filterRule;
    }
    
    if(layer == "L4Simple") {
        auto rule = this->parseSimpleL4Rule();
        filterRule = std::make_shared<FilterRule>(rule,RID);
        return filterRule;
    }

    throw std::runtime_error("Could not find firewall rule in CLI args\n");
}


std::string CLIParser::parseLayer() {
    for(int i = 0; i < this->argc - 1; ++i) {
        if(!std::strcmp(this->argv[i],"-l")) {
            return std::string(this->argv[i+1]);
        }   
    }
    throw std::runtime_error("Layer wasnt found\n");
}

bool CLIParser::parseAction() {
    bool permit = false; 

    for(int i = 0; i < this->argc - 1; ++i) {
        if (std::strcmp(this->argv[i], "-action") == 0) {
            permit = (std::strcmp(this->argv[i+1], "permit") == 0);
        }
    }

    return permit;
}


void CLIParser::parseIP(std::tuple<std::string,int>& address,const char* target) {
    std::stringstream ss(target);
    std::string ip;
    std::string prefix;

    std::getline(ss,ip,'/');
    std::getline(ss,prefix,'/');
    int prefixNum = std::stoi(prefix);

    address = std::make_tuple(ip,prefixNum);
}

std::shared_ptr<L3Rule> CLIParser::parseL3Rule() {
    if (this->argc < 5) {
        throw std::runtime_error("Invalid CLI format (too few arguments)\n");
    }
    bool permit = this->parseAction();
    std::tuple<std::string,int> sourceA{"none",-1};
    std::tuple<std::string,int> destA{"none",-1};
    int ttlMax = -1;
    int ttlMin = -1;

    const auto [protocol,TOS,allowFrags] = this->parseIPINFO();

    for(int i = 0; i < this->argc - 1; ++i) {
        if(!std::strcmp(this->argv[i],"-sa")) {
            this->parseIP(sourceA, this->argv[i+1]);
        }
        if(!std::strcmp(this->argv[i],"-da")) {
            this->parseIP(sourceA, this->argv[i+1]);
        }
        if(!std::strcmp(this->argv[i],"-ttlMax")) {
            ttlMax = std::atoi(this->argv[i+1]);
        }
        if(!std::strcmp(this->argv[i],"-ttlMin")) {
            ttlMin = std::atoi(this->argv[i+1]);
        }
    }
    return std::make_shared<L3Rule>(permit, -1, sourceA, destA,ttlMax,ttlMin,protocol,TOS,allowFrags);

}