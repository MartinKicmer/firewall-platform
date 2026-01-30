#include "../headers/PacketBlockerGateway.h"
#include <boost/process/detail/child_decl.hpp>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <string>

nlohmann::json PacketBlockerGateway::getLastPDUS(int count,const std::string& layer,bool permit) {
    boost::process::ipstream outStream;
    //-l L3 -action deny redirect 10
    nlohmann::json jsonArr = nlohmann::json::array();
    try {
        boost::process::child pr(this->processPath,
            "-l",layer.c_str(),"-action",permit ? "permit" : "deny","redirect",std::to_string(count),boost::process::std_out > outStream);

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
        boost::process::ipstream outStream;
        boost::process::child pr(this->processPath,"remove","-rid",std::to_string(ID),"-l",layer,
        "-memory",std::to_string(fromMemory));
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




void PacketBlockerGateway::createL2Rule(int ID,bool permit,const std::string& sourceMAC,const std::string& destMAC,bool save) {
    try {
        boost::process::child pr;
        if(save) {
             pr = boost::process::child(this->processPath,"-rid",std::to_string(ID),
        "-l","L2","-action",permit ? "permit" : "deny","-smac",sourceMAC,"-dmac",destMAC,"-save");
        } else {
            pr = boost::process::child(this->processPath,"-rid",std::to_string(ID),
        "-l","L2","-action",permit ? "permit" : "deny","-smac",sourceMAC,"-dmac",destMAC);
        }
        pr.wait();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}