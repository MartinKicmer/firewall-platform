//
// Created by andrew on 4/22/26.
//
#pragma once
#include <memory>
#include <fstream>
class FirewallService;
class DebugHandler {
public:
    DebugHandler(const std::string& filename_) : fileName(filename_) {}
    void handleLogging(FirewallService* firewallService);
private:
    std::string fileName;
};