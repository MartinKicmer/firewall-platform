#pragma once
#include "../../firewall-service/headers/Rules.h"
#include "../../firewall-service/headers/FilterRule.h"
#include <memory>
#include <stdexcept>   
#include <cstring>
#include <iostream>

class CLIParser {
public:
    CLIParser(int argc,const char** argv) 
    : argv(argv),argc(argc) {}

    std::shared_ptr<FilterRule> parseCLIArguments();

private:
    const char** argv;
    const int argc;

    [[nodiscard]] std::shared_ptr<L2Rule> parseL2Rule();
    [[nodiscard]] int parseRID();
};