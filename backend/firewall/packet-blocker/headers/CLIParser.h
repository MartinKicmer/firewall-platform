#pragma once
#include "../../firewall-service/headers/Rules.h"
#include "../../firewall-service/headers/FilterRule.h"
#include <memory>
#include <stdexcept>   
#include <cstring>
#include <iostream>
#include <tuple>
#include <unordered_map>
class CLIParser {
public:
    CLIParser(int argc,const char** argv) 
    : argv(argv),argc(argc) {
        this->protocols["UDP"] = 17;
        this->protocols["ICMP"] = 1;
        this->protocols["TCP"] = 6;
    }
    std::shared_ptr<FilterRule> parseCLIArguments();

private:
    const char** argv;
    const int argc;
    std::unordered_map<std::string,int> protocols;


    bool containsRedirect();
    bool containsSave();
    bool containsSelect();

    std::shared_ptr<SelectRule> parseSelectRule();
    [[nodiscard]] std::shared_ptr<L2Rule> parseL2Rule();
    [[nodiscard]] std::shared_ptr<L3Rule> parseL3Rule();
    [[nodiscard]] std::shared_ptr<RedirectRule> parseRedirectRule();

    [[nodiscard]] bool parseAction();
    [[nodiscard]] int parseRID();
    [[nodiscard]] std::string parseLayer();
    [[nodiscard]] std::tuple<int,int,bool> parseIPINFO();

    void parseIP(std::tuple<std::string,int>& address,const char* target);
};