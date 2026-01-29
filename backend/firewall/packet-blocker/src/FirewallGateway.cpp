#include "../headers/FirewallGateway.h"
#include <memory>

void FirewallGateway::printRedirectedPackets() {
     this->helper = std::thread([]() { 
                MQConnector reader("/firewallRedirect");
                reader.create(); 
                while(true) {
                    const auto& [isEnd, data] = reader.recieveData();
                    if(isEnd) {
                        //std::cout << "Redirect finished (end received)" << std::endl;
                        break; 
                    }
                    try {
                        auto json = nlohmann::json::parse(data);
                        std::cout << json.dump() << std::endl;
                    } catch (const std::exception& e) {
                        std::cerr << e.what() << std::endl;
                        break;
                    }
                }
                reader.close();
            });
}



void FirewallGateway::sendRule(std::shared_ptr<FilterRule> rule) {

    if(auto redirectRule = std::dynamic_pointer_cast<RedirectRule>(rule->getRule())){
           this->printRedirectedPackets();
    }

    std::string seriliazedJSON = rule->serializeToJSON();
    std::cout << seriliazedJSON << std::endl;
    MQConnector connector(this->mqPath);
    connector.connect();
    connector.sendData(seriliazedJSON);
    connector.close();

    if(this->helper.joinable()) this->helper.join();
}