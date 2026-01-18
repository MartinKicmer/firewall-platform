#include "../headers/CLIParser.h"

int CLIParser::parseRID() {
    if(this->argc < 3) throw std::runtime_error("Invalid CLI number of args\n");
    for (int i = 0; i < this->argc - 1; ++i) {
        if (!std::strcmp(this->argv[i], "-rid")) {
            return std::stoi(this->argv[i + 1]);
        }
    }
    throw std::runtime_error("Could not find RID argument (-rid)\n");
}
std::shared_ptr<L2Rule> CLIParser::parseL2Rule() {
    if (this->argc < 5) {
        throw std::runtime_error("Invalid CLI format (too few arguments)\n");
    }

    std::string smac = "none";
    std::string dmac = "none";
    bool permit = false; 

    for (int i = 0; i < this->argc - 1; ++i) {
        if (std::strcmp(this->argv[i], "-smac") == 0) {
            smac = this->argv[i + 1];
        } else if (std::strcmp(this->argv[i], "-dmac") == 0) {
            dmac = this->argv[i + 1];
        } else if (std::strcmp(this->argv[i], "-action") == 0) {
            permit = (std::strcmp(this->argv[i+1], "permit") == 0);
        }
    }

    
    return std::make_shared<L2Rule>(permit, -1, smac, dmac);
}

std::shared_ptr<FilterRule> CLIParser::parseCLIArguments() {
    std::shared_ptr<FilterRule> filterRule;
    int RID = this->parseRID();
    for(int i = 0; i < this->argc - 1; ++i) {
        auto current = this->argv[i];
        if(!std::strcmp(current,"-l")) {
            current = this->argv[i+1];
            if(!std::strcmp(current,"L2")) {
                auto rule = this->parseL2Rule();
                filterRule = std::make_shared<FilterRule>(rule,RID);
                return filterRule;
            }
        }
    }

    throw std::runtime_error("Could not find firewall rule in CLI args\n");
}