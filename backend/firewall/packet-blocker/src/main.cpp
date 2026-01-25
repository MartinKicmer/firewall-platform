#include "../headers/CLIParser.h"
#include "../../firewall-service/headers/MQConnector.h"
#include <exception>
#include <memory>
#include <string>
#include <thread>

int main(int argc, const char** argv) {
    try {
        CLIParser parser(argc,argv);
        auto filterRule = parser.parseCLIArguments();
        std::thread th;
        if(auto redirectRule = std::dynamic_pointer_cast<RedirectRule>(filterRule->getRule())){
            th = std::thread([]() { 
                MQConnector reader("/firewallRedirect");
                reader.create(); 
                while(true) {
                    const auto& [isEnd, data] = reader.recieveData();
                    if(isEnd) {
                        std::cout << "Redirect finished (end received)" << std::endl;
                        break; 
                    }
                    try {
                        auto json = nlohmann::json::parse(data);
                        std::cout << "Received redirect: " << json.dump(4) << std::endl;
                    } catch (const std::exception& e) {
                        std::cerr << e.what() << std::endl;
                        break;
                    }
                }
                reader.close();
            });
        }
        std::string seriliazedJSON = filterRule->serializeToJSON();
        std::cout << seriliazedJSON << std::endl;
        MQConnector connector("/firewallQueue");
        connector.connect();
        connector.sendData(seriliazedJSON);
        connector.close();

        th.join();

    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        std::exit(EXIT_FAILURE);
    }
    
    std::exit(EXIT_SUCCESS);
}
