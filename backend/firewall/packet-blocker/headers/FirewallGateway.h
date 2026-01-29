#pragma once
#include <memory>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include "../../firewall-service/headers/FilterRule.h"
#include "../../firewall-service/headers/MQConnector.h"
#include <thread>
class FirewallGateway {

public:
    FirewallGateway(const std::string mqPath_) : mqPath(mqPath_) {}
    void sendRule(std::shared_ptr<FilterRule> rule);

private:
    std::string mqPath;
    std::thread helper;

    void printRedirectedPackets();
};


