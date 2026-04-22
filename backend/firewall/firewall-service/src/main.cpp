#include <cstdlib>
#include <cstring>
#include <iostream>
#include "../headers/FirewallService.h"
int main(int argc,const char** argv) {

    int noQueues = 1;
    bool debug = false;
    for (int i = 0; i < argc; ++i) {
        if (!std::strcmp(argv[i],"-debug")) debug = true;
        if (!std::strcmp(argv[i],"-queues")) {
            if (argv[i+1] == nullptr) {
                std::cerr<<"Error: -queues must be specified"<<std::endl;
                std::exit(EXIT_FAILURE);
            }
            noQueues = std::stoi(argv[i+1]);
        }
    }
    std::cout << "Params: " << noQueues << ", " << debug << std::endl;
    FirewallService service(debug,noQueues);
    if(argc > 1) {
        if(!std::strcmp(argv[1],"clearDB")) {
            auto logger = FilterRuleLogger::getInstance();
            logger.clearAllRules();
        }   
    }
    service.run("../firewall_config.txt");

    std::exit(EXIT_SUCCESS);
}