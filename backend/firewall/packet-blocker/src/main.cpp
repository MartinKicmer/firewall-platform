#include "../headers/CLIParser.h"
#include "../../firewall-service/headers/MQConnector.h"

int main(int argc, const char** argv) {
    try {
        CLIParser parser(argc,argv);
        auto filterRule = parser.parseCLIArguments();
        std::string seriliazedJSON = filterRule->serializeToJSON();
        MQConnector connector("/firewallQueue");
        connector.connect();
        connector.sendData(seriliazedJSON);

        connector.close();

    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        std::exit(EXIT_FAILURE);
    }
    
    std::exit(EXIT_SUCCESS);
}
