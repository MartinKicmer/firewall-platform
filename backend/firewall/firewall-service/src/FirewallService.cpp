#include "../headers/FirewallService.h"
#include "../headers/KernelSocket.h"
#include "../headers/PacketParser.h"
#include <iterator>
#include <memory>


#include "../headers/FirewallService.h"
#include "../headers/KernelSocket.h"
#include "../headers/PacketParser.h"
#include <memory>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <netinet/in.h>

static bool END_DEBUG = false;
void debug_handler(int signum) {
    END_DEBUG = true;

}


bool FirewallService::STOP_DEBUG() {
    return END_DEBUG;
}


int FirewallService::handlePacketCallback(struct nfq_q_handle* qh,
                                          struct nfgenmsg* nfmsg,
                                          struct nfq_data* nfa,
                                          void* data) {
    FirewallService* fw = static_cast<FirewallService*>(data);

    uint32_t id = 0;
    struct nfqnl_msg_packet_hdr* ph = nfq_get_msg_packet_hdr(nfa);
    if (ph) {
        id = ntohl(ph->packet_id);
    } else {
        return 0; 
    }

    unsigned char* payload;
    int len = nfq_get_payload(nfa, &payload);
    

    bool permit = true;

    if (len >= 0) {
        try {
            fw->packetParser->initParser(payload,len);
            fw->filterList->setParser(fw->packetParser);
            if (fw->filterList->getRules().empty() && !fw->packetRedirector->canRedirect() ) {
                if (fw->debugModeActive()) {
                    auto& logBuf = fw->getLogBuffer();
                    auto record = fw->packetParser->getCombinedRecord(true);
                    logBuf.write(record);
                }
                return nfq_set_verdict(qh, id, permit ? NF_ACCEPT : NF_DROP, 0, nullptr);
            }
            auto blockingRule = fw->filterList->checkAllRules();
            if (blockingRule != nullptr) {
                permit = blockingRule->shouldIgnore() ? true : false;
            }
            if (fw->debugModeActive()) {
                auto& logBuf = fw->getLogBuffer();
                auto record = fw->packetParser->getCombinedRecord(permit);
                logBuf.write(record);
            }
            if (fw->packetRedirector->canRedirect()) {
                if (blockingRule) {
                    if (blockingRule->shouldIgnore()) {
                        return nfq_set_verdict(qh, id, permit ? NF_ACCEPT : NF_DROP, 0, nullptr);
                    }
                }
                auto ruleWrap = fw->packetRedirector->getRule();
                if (ruleWrap && ruleWrap->getRule()) {
                    if ((ruleWrap->getRule()->permit && !blockingRule) ||
                        (!ruleWrap->getRule()->permit && blockingRule)) {
                            fw->packetRedirector->redirectPacket(fw->packetParser);
                        }
                }

            }

        } catch (const std::exception& e) {
            std::cerr << "Parser error (ignoring for safety): " << e.what() << std::endl;
            std::exit(EXIT_FAILURE);
        } catch (...) {
            std::cerr << "Unknown critical error in callback logic!" << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
    return nfq_set_verdict(qh, id, permit ? NF_ACCEPT : NF_DROP, 0, nullptr);
}


FirewallService::~FirewallService() {
    if(this->packetBlockerT.joinable()) {
        this->packetBlockerT.join();
    }
}



void FirewallService::deleteKernelSocket(KernelSocket *ks) {
        delete ks;
}

FirewallService::FirewallService(bool debug_,int noQueues_)
            : config(nullptr), redirect(false),debug(debug_),selectedNoQueues(noQueues_) {
    try {
        if (debug_) {
            std::cout << "DEBUG MODE ACTIVE" << std::endl;
            this->debugHandler = std::make_shared<DebugHandler>("../debug.bin");
            this->debugThread  = std::thread(&DebugHandler::handleLogging,this->debugHandler,this);
        }
        std::signal(SIGINT, debug_handler);
        this->filterList = std::make_shared<FilterRuleList>();
        this->packetBlockerT = std::thread(&FirewallService::startPacketBlockerCommunication,this);
        this->packetRedirector = std::make_shared<PacketRedirector>();
        this->packetBlockerGateway = std::make_unique<PacketblockerGateway>("/fireWallBlocker",this->filterList);
        for (int i = 0; i < this->selectedNoQueues; ++i) {
            auto sock = std::make_unique<KernelSocket>(this, i);
            this->kernelThreads.emplace_back(&KernelSocket::run, sock.get());
            this->kernelSockets.push_back(std::move(sock));
        }
        this->packetParser = std::make_shared<PacketParser>();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

void FirewallService::loadSavedRules() {
    auto logger = FilterRuleLogger::getInstance();
    auto rules = logger.selectAllRules();
    for(auto rule : rules) {
        rule->printRule();
        this->filterList->addRule(rule);
    }
}

void FirewallService::run(const std::string& standardPath) {
    try {
        this->config = this->loadFromConfig(standardPath);
        this->loadSavedRules();
        std::cout << this->config << std::endl;
        for (auto& t : this->kernelThreads) {
            if (t.joinable()) {
                t.join(); 
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


void FirewallService::removeRuleFromMemory(std::shared_ptr<FilterRule> rule) {
    this->filterList->removeRule(rule);
}

void FirewallService::writePacketBlockerData(const std::string& data) {
    std::cout << "Adding rule: " << data << std::endl;
    auto deserializedRule = FilterRule::deserialize(data);
    if(deserializedRule->canSave()) {
        auto logger = FilterRuleLogger::getInstance();
        logger.log(deserializedRule);
    }
    if(auto removeRule = std::dynamic_pointer_cast<RemoveRule>(deserializedRule->getRule())) {
        auto logger = FilterRuleLogger::getInstance();
        if(!removeRule->fromMemory) logger.removeRuleByID(deserializedRule);
        else this->filterList->removeRule(deserializedRule);
        return;
    }
    if(auto redirectRule = std::dynamic_pointer_cast<RedirectRule>(deserializedRule->getRule())) {
        this->packetRedirector->setRule(deserializedRule);
        return;
    }
    if(auto selectRule = std::dynamic_pointer_cast<SelectRule>(deserializedRule->getRule())) {
        this->packetBlockerGateway->sendSelectedRules(deserializedRule);
        return;
    }
    if(deserializedRule->canUpdate()) {
          auto logger = FilterRuleLogger::getInstance();
          logger.updateRule(deserializedRule);
    }
    this->filterList->addRule(deserializedRule);
}
