#pragma once
#include <cstdlib>
#include <exception>
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
#include "FilterRuleList.h"
#include "MQConnector.h"
#include "PacketRedirector.h"
#include <atomic>
#include "FilterRuleLogger.h"
#include <libnetfilter_queue/libnetfilter_queue.h>
#include <shared_mutex>
#include <csignal>
#include "LoggingBuffer.h"
#include "DebugHandler.h"
#ifndef NF_ACCEPT
#define NF_ACCEPT 1
#endif

#ifndef NF_DROP
#define NF_DROP 0
#endif
class KernelSocket;
class PacketParser;
class FirewallService {
public:
    struct LogBuffer {

    };
    struct Interface {
        std::string name;
        std::string state;
    };
    struct Config {
        std::vector<Interface> activeInterfaces;

        friend std::ostream& operator<<(std::ostream& o, const std::unique_ptr<Config>& config_) {
            o << "Running Config\n";
            o << "Active interfaces\n";
            for(const auto& i : config_->activeInterfaces) {
                o << "interface: " << i.name << " state: " << i.state << std::endl;
            }
            return o;

        }

    };
    FirewallService(bool debug_,int noQueues_);
    ~FirewallService();
    void run(const std::string& standardPath);
    void loadSavedRules();
    void removeRuleFromMemory(std::shared_ptr<FilterRule> rule);

    static int handlePacketCallback(struct nfq_q_handle* qh,
                        struct nfgenmsg* nfmsg,
                        struct nfq_data* nfa,
                        void* data);

    static void deleteKernelSocket(KernelSocket* ks);

    LoggingBuffer& getLogBuffer() { return  this->logBuffer; }
    [[nodiscard]] bool debugModeActive() const { return this->debug; }
    static bool STOP_DEBUG();
private:
    [[nodiscard]] std::unique_ptr<FirewallService::Config> loadFromConfig(const std::string& standardPath) const;
    void startPacketBlockerCommunication();
    void writePacketBlockerData(const std::string& data);
    std::vector<std::unique_ptr<KernelSocket>> kernelSockets; 
    std::vector<std::thread> kernelThreads;
    std::unique_ptr<Config> config = nullptr;
    std::thread packetBlockerT;
    bool redirect;
    std::shared_ptr<FilterRuleList> filterList = nullptr;
    std::shared_ptr<PacketRedirector> packetRedirector = nullptr;
    std::unique_ptr<PacketblockerGateway> packetBlockerGateway = nullptr;
    std::shared_ptr<PacketParser> packetParser = nullptr;
    bool debug = false;
    int selectedNoQueues = 1;
    LoggingBuffer logBuffer;
    std::shared_ptr<DebugHandler> debugHandler = nullptr;
    std::thread debugThread;
};