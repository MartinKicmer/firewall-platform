#pragma once
#include <string>
#include <vector>
#include <thread>
#include <cstring>
#include <fstream>
#include <array>
#include <sstream>
#include <iostream>
#include "RawSocket.h"
#include "PacketParser.h"
#include "FilterRuleList.h"
#include <atomic>
class FirewallService {
public:
    struct Interface {
        std::string name;
        std::string state;
    };
    struct Config {
        std::vector<Interface> activeInterfaces;

        friend std::ostream& operator<<(std::ostream& o, const std::unique_ptr<Config>& config) {
            o << "Running Config\n";
            o << "Active interfaces\n";
            for(const auto& i : config->activeInterfaces) {
                o << "interface: " << i.name << " state: " << i.state << std::endl;
            }
            return o;

        }

        void bindFirstActiveInterface(RawSocket& rsocket) {
            for(const auto& i : this->activeInterfaces) {
                if(i.state == "input") {
                    rsocket.bindSocket(i.name);
                    break;
                }
            }
        }
    };
    FirewallService() : config(nullptr) {
        this->rawSocket.initSocket();
        this->filterList = std::make_shared<FilterRuleList>();
        std::shared_ptr<L2Rule> l2rule = std::make_shared<L2Rule>(false,10,"none","ff:ff:ff:ff:ff:ff");
        this->filterList->addRule(1,l2rule);
    }
    void run(const std::string& standardPath);
private:
    [[nodiscard]] std::unique_ptr<FirewallService::Config> loadFromConfig(const std::string& standardPath) const;
    void packetBlockerCommunicator();    
    std::unique_ptr<Config> config;
    std::thread packetBlockerT;
    RawSocket rawSocket;
    std::shared_ptr<FilterRuleList> filterList;

};