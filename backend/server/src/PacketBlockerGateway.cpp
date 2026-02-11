#include "../headers/PacketBlockerGateway.h"
#include <boost/process/detail/child_decl.hpp>
#include <cstdlib>
#include <exception>
#include <memory>
#include <netinet/tcp.h>
#include <stdexcept>
#include <string>


void PacketBlockerGateway::createL4TCPRule(L4TCPRequest req,bool permit) {
    std::string flag;
    if(req.flags & TH_SYN) flag = "SYN";
    if(req.flags & TH_FIN) flag = "FIN";
    if(req.flags & TH_ACK) flag = "ACK";
    try {
        boost::process::child pr;
        if(req.save) {
             pr = boost::process::child(this->processPath,"-rid",std::to_string(req.ID),
        "-l","L4TCP","-action",permit ? "permit" : "deny","-sport",std::to_string(req.sPort),"-dport",std::to_string(req.dPort),"-save","-flag",flag,"-minWin",std::to_string(req.minWindowSize),"-maxWin",std::to_string(req.maxWindowSize));
        } else {
            pr = boost::process::child(this->processPath,"-rid",std::to_string(req.ID),
        "-l","L4TCP","-action",permit ? "permit" : "deny","-sport",std::to_string(req.sPort),"-dport",std::to_string(req.dPort),"-flag",flag,"-minWin",std::to_string(req.minWindowSize),"-maxWin",std::to_string(req.maxWindowSize));
        }
        pr.wait();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}
void PacketBlockerGateway::printRedirectedPackets(std::shared_ptr<WebSocketService> wsService) {
    while (true) {
        boost::process::ipstream outStream;
        try {
            std::string currentLayer;
            bool currentPermit;
            {
                std::lock_guard<std::mutex> lock(params.mtx);
                currentLayer = params.layer;
                currentPermit = params.permit;
            }

            auto child = boost::process::child(
                this->processPath,
                "-l", currentLayer,
                "-action", currentPermit ? "permit" : "deny",
                "redirect", "all",
                boost::process::std_out > outStream
            );
            
            this->prActive = std::make_unique<boost::process::child>(std::move(child));

            std::string line;
            while (!this->restart && std::getline(outStream, line)) {
                if (line.empty()) continue;
                wsService->broadcastMSG(line);
            }

            if (this->prActive && this->prActive->running()) {
                this->prActive->terminate();
                this->prActive->wait();
            }

        } catch (const std::exception& e) {
            std::cerr << "CRITICAL ERROR v printRedirectedPackets: " << e.what() << std::endl;
            std::cerr << "Path to packet blocker: " << this->processPath << std::endl;
            
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }
}

nlohmann::json PacketBlockerGateway::getLastPDUS(int count,const std::string& layer,bool permit) {
    boost::process::ipstream outStream;
    //-l L3 -action deny redirect 10
    nlohmann::json jsonArr = nlohmann::json::array();
    try {
        boost::process::child pr(this->processPath,
            "-l",layer.c_str(),"-action",permit ? "permit" : "deny","redirect",count != -1 ? std::to_string(count) : "all",boost::process::std_out > outStream);

        std::string line;
        while (std::getline(outStream, line)) {
            if(line.empty()) continue;
            try {
                jsonArr.push_back(nlohmann::json::parse(line));
            } catch(...) {
                continue;
            }
        }

        pr.wait();

        return jsonArr;
    }  catch (const std::exception& e) {
        std::cerr << "Eror while trying to capture last PDUS: " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}


void PacketBlockerGateway::removeRule(int ID,const std::string& layer,bool fromMemory) {
     try {
        boost::process::child pr(this->processPath,"remove","-rid",std::to_string(ID),"-l",layer,
        "-memory",fromMemory ? "true" : "false");
        pr.wait();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

nlohmann::json PacketBlockerGateway::getSelectedRules(int ID,bool permit,const std::string& layer,bool fromMemory) {
    try {
        boost::process::ipstream outStream;
        boost::process::child pr(this->processPath,"select","-rid",std::to_string(ID),"-l",layer,"-action",permit ? "permit" : "deny",
        "-memory",std::to_string(fromMemory),boost::process::std_out > outStream);
        std::string line;
        std::string fullJSON;
        while (std::getline(outStream, line)) {
            if(line.empty()) continue;
            fullJSON.append(line);
        }
        std::cout << fullJSON << std::endl;
        nlohmann::json arr = nlohmann::json::parse(fullJSON);
        if(!arr.is_array()) {
            throw std::runtime_error("JSON is not an array\n");
        }
        pr.wait();
        return arr;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

void PacketBlockerGateway::createL4simpleRule(L4SimpleRequest req,bool permit) {
     try {
        boost::process::child pr;
        if(req.save) {
             pr = boost::process::child(this->processPath,"-rid",std::to_string(req.ID),
        "-l","L4Simple","-action",permit ? "permit" : "deny","-sport",std::to_string(req.sPort),"-dport",std::to_string(req.dPort),"-save");
        } else {
            pr = boost::process::child(this->processPath,"-rid",std::to_string(req.ID),
        "-l","L4Simple","-action",permit ? "permit" : "deny","-sport",std::to_string(req.sPort),"-dport",std::to_string(req.dPort));
        }
        pr.wait();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

void PacketBlockerGateway::createL3Rule(IPv4Request req,bool permit) {
    try {
        // -rid 1 -l L3 -action deny -sa ../prefix -da ../prefix -ttlMax ... -ttlMin ... -proto ... -allowFrag ... -tos ...
        boost::process::child pr;
        const auto& [source,sourcePref] = req.source;
        const auto& [dest,destPref] = req.dest;
        if(req.save) {
             pr = boost::process::child(this->processPath,"-rid",std::to_string(req.ID),
        "-l","L3","-action",permit ? "permit" : "deny","-sa",
        source + "/" + std::to_string(sourcePref),dest + "/" + std::to_string(destPref),
         "-ttlMin",std::to_string(req.ttlMin),"-ttlMax",std::to_string(req.ttlMax),"-proto",req.proto,"-allowFrag",std::to_string(req.allowFragments),"-tos",std::to_string(req.tos),"-save");
        } else {
               pr = boost::process::child(this->processPath,"-rid",std::to_string(req.ID),
        "-l","L3","-action",permit ? "permit" : "deny","-sa",
        source + "/" + std::to_string(sourcePref),dest + "/" + std::to_string(destPref),
         "-ttlMin",std::to_string(req.ttlMin),"-ttlMax",std::to_string(req.ttlMax),"-proto",req.proto,"-allowFrag",std::to_string(req.allowFragments),"-tos",std::to_string(req.tos));
        }
        pr.wait();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

void PacketBlockerGateway::createL2Rule(EthernetRequest req,bool permit) {
    try {
        boost::process::child pr;
        if(req.save) {
             pr = boost::process::child(this->processPath,"-rid",std::to_string(req.ID),
        "-l","L2","-action",permit ? "permit" : "deny","-smac",req.source,"-dmac",req.dest,"-save");
        } else {
            pr = boost::process::child(this->processPath,"-rid",std::to_string(req.ID),
        "-l","L2","-action",permit ? "permit" : "deny","-smac",req.source,"-dmac",req.source);
        }
        pr.wait();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}