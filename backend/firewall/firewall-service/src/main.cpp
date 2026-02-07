#include <cstdlib>
#include <cstring>
#include <iostream>
#include "../headers/FirewallService.h"
int main(int argc,const char** argv) {

    FirewallService service;
    if(argc > 1) {
        if(!std::strcmp(argv[1],"clearDB")) {
            auto logger = FilterRuleLogger::getInstance();
            logger.clearAllRules();
        }   
    }
    service.run("../firewall_config.txt");

    std::exit(EXIT_SUCCESS);
}