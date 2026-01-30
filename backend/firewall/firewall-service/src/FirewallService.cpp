#include "../headers/FirewallService.h"
#include <memory>


void FirewallService::loadSavedRules() {
    auto logger = FilterRuleLogger::getInstance();
    auto rules = logger.selectAllRules();
    for(auto rule : rules) {
        std::cout << rule->getRule() << std::endl;
        this->filterList->addRule(rule);
    }
}

void FirewallService::run(const std::string& standardPath) {
    try {
        this->config = this->loadFromConfig(standardPath);
        this->config->bindFirstActiveInterface(this->rawSocket);
        this->loadSavedRules();
        std::cout << this->config << std::endl;
       while(1) {
        this->rawSocket.readFromSocket();
        auto& data = this->rawSocket.getReadData();
        
        auto packetParser = std::make_shared<PacketParser>(data);
        this->filterList->setParser(packetParser);
        packetParser->printL4Layer(PacketParser::PduType::UDPDATAGRAM);
        auto blockingRule = this->filterList->checkAllRules();
        if(blockingRule != nullptr) {
            std::cout << "!!! PACKET BLOCKED !!!" << std::endl;
            this->filterList->printFilterRuleInfo(blockingRule);
        } else {
            //std::cout << "Packet passed" << std::endl;
        }

        if(this->packetRedirector->canRedirect()) {
            auto rule = this->packetRedirector->getRule();
            if(rule->getRule()->permit && !blockingRule) {
                this->packetRedirector->redirectPacket(packetParser);
            }
            if(!rule->getRule()->permit && blockingRule) {
                this->packetRedirector->redirectPacket(packetParser);
            }
        }
}
    } catch( const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}



std::unique_ptr<FirewallService::Config> FirewallService::loadFromConfig(const std::string& standardPath) const {
    std::array<std::string,1> keywords = {"interface: "};
    std::unique_ptr<FirewallService::Config> loadedConfig = std::make_unique<FirewallService::Config>();
    std::vector<FirewallService::Interface> interfaces;
    std::ifstream ifile(standardPath);
    if(ifile.is_open()) {
        std::string line;
        while(std::getline(ifile,line,'\n')) {
            for(int i = 0; i < 1; i++){
                std::size_t firstPos = line.find(keywords[i]);
                if(firstPos != std::string::npos) {
                    if(keywords[i] == "interface: " ) {
                        firstPos = firstPos + keywords[i].length();
                        int lastPos = line.find(',');
                        std::string intName = line.substr(firstPos,lastPos - firstPos);
                        
                        firstPos = line.find(',')+1;
                        std::string state = line.substr(firstPos,line.length()-1);

                        Interface interface = {
                            .name = intName,
                            .state = state
                        };
                        interfaces.push_back(interface);
                    }
                    
                }

            }

        }
        loadedConfig->activeInterfaces = interfaces;
        ifile.close();

        return loadedConfig;
    } else {
        throw std::runtime_error("Could not open config");
    }

}

void FirewallService::startPacketBlockerCommunication() {
    try {
        std::cout << "Communication thread started\n";
        MQConnector mqconnector("/firewallQueue");
        mqconnector.create();
        std::cout << "Message queue created\n";
        while(true) {
            const auto& [end,data] = mqconnector.recieveData();
            if(end) {
                std::cout << "Communication thread ended\n";
                break;
            }
            this->writePacketBlockerData(data);
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "Ending communication thread\n";
        return;
    }
}

void FirewallService::writePacketBlockerData(const std::string& data) {
    std::cout << "Adding rule: " << data << std::endl;
    auto deserializedRule = FilterRule::deserialize(data);
    if(deserializedRule->canSave()) {
        auto logger = FilterRuleLogger::getInstance();
        logger.log(deserializedRule);
    }
    if(auto redirectRule = std::dynamic_pointer_cast<RedirectRule>(deserializedRule->getRule())) {
        this->packetRedirector->setRule(deserializedRule);
        return;
    }
    if(auto selectRule = std::dynamic_pointer_cast<SelectRule>(deserializedRule->getRule())) {
        this->packetBlockerGateway->sendSelectedRules(deserializedRule);
        return;
    }

    this->filterList->addRule(deserializedRule);
}
