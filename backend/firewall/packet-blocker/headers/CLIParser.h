#pragma once
#include "../../firewall-service/headers/Rules.h"
#include "../../firewall-service/headers/FilterRule.h"
#include <memory>
#include <stdexcept>   
#include <cstring>
#include <iostream>
#include <tuple>

class CLIParser {
public:
    CLIParser(int argc,const char** argv) 
    : argv(argv),argc(argc) {}

    std::shared_ptr<FilterRule> parseCLIArguments();

private:
    const char** argv;
    const int argc;

    bool containsRedirect();

    [[nodiscard]] std::shared_ptr<L2Rule> parseL2Rule();
    [[nodiscard]] std::shared_ptr<L3Rule> parseL3Rule();
    [[nodiscard]] std::shared_ptr<RedirectRule> parseRedirectRule();

    [[nodiscard]] bool parseAction();
    [[nodiscard]] int parseRID();
    [[nodiscard]] std::string parseLayer();
    [[nodiscard]] std::tuple<int,int,bool> parseIPINFO();

    void parseIP(std::tuple<std::string,int>& address,const char* target);
};