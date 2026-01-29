#include "../headers/CLIParser.h"
#include "../headers/FirewallGateway.h"

int main(int argc, const char** argv) {
    try {
        CLIParser parser(argc,argv);
        auto filterRule = parser.parseCLIArguments();
        FirewallGateway firewallGateway("/firewallQueue");
        firewallGateway.sendRule(filterRule);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        std::exit(EXIT_FAILURE);
    }
    
    std::exit(EXIT_SUCCESS);
}
