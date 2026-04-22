#pragma once
#include <boost/process.hpp>
#include <iostream>
#include <atomic>
#include <mutex>
#include "RequestTypes.h"
#include <nlohmann/json.hpp>
#include "WebSocketService.h"
class PacketBlockerGateway {
public:
    struct StreamParams {
        std::string layer = "L3";
        bool permit = true;
        std::mutex mtx;
    };

    void setStreamParams(const std::string& layer, bool permit) {
        {
            std::lock_guard<std::mutex> lock(params.mtx);
            params.layer = layer;
            params.permit = permit;
            restart.store(true);
        }

        std::lock_guard<std::mutex> lock(processMtx);
        if (prActive && prActive->running()) {
            prActive->terminate();
            prActive->wait();
        }
    }

    PacketBlockerGateway(const std::string& processPath_)
        : processPath(processPath_) {}

    nlohmann::json getLastPDUS(int count, const std::string& layer, bool permit);
    void createL2Rule(EthernetRequest req, bool permit);
    void createL3Rule(IPv4Request req, bool permit);
    void createL4simpleRule(L4SimpleRequest req, bool permit);
    void createL4TCPRule(L4TCPRequest req, bool permit);
    nlohmann::json getSelectedRules(int ID, bool permit, const std::string& layer, bool fromMemory);
    void removeRule(int ID, const std::string& layer, bool fromMemory);

    void printRedirectedPackets(std::shared_ptr<WebSocketService> wsService);

private:
    std::string processPath;
    StreamParams params;
    std::atomic<bool> restart{false};
    std::unique_ptr<boost::process::child> prActive;
    std::mutex processMtx;
};
