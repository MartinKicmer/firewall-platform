#pragma once
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <cstring>
#include <fstream>
#include <array>
#include <sstream>
#include <iostream>
#include "FilterRule.h"
#include "PacketblockerGateway.h"
#include "RawSocket.h"
#include "PacketParser.h"
#include "FilterRuleList.h"
#include "MQConnector.h"
#include "PacketRedirector.h"
#include <atomic>
#include "FilterRuleLogger.h"
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
    FirewallService() : config(nullptr),redirect(false) {
        this->rawSocket.initSocket();
        this->filterList = std::make_shared<FilterRuleList>();
        this->packetBlockerT = std::thread(&FirewallService::startPacketBlockerCommunication,this);
        this->packetRedirector = std::make_shared<PacketRedirector>();
        this->packetBlockerGateway = std::make_unique<PacketblockerGateway>("/fireWallBlocker",this->filterList);
    }
    ~FirewallService() {
        if(this->packetBlockerT.joinable()) {
            this->packetBlockerT.join();
        }
    }
    void run(const std::string& standardPath);
    void loadSavedRules();
    void removeRuleFromMemory(std::shared_ptr<FilterRule> rule);
private:
    [[nodiscard]] std::unique_ptr<FirewallService::Config> loadFromConfig(const std::string& standardPath) const;
    void startPacketBlockerCommunication();
    void writePacketBlockerData(const std::string& data);

    std::unique_ptr<Config> config;
    std::thread packetBlockerT;
    RawSocket rawSocket;
    bool redirect;
    std::shared_ptr<FilterRuleList> filterList;
    std::shared_ptr<PacketRedirector> packetRedirector;
    std::unique_ptr<PacketblockerGateway> packetBlockerGateway;
};