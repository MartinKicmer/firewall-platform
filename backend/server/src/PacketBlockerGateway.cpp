#include "../headers/PacketBlockerGateway.h"
#include <boost/process/detail/child_decl.hpp>
#include <cstdlib>
#include <exception>
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

void PacketBlockerGateway::createL2Rule(int ID,bool permit,const std::string& sourceMAC,const std::string& destMAC) {
    try {
        boost::process::child pr(this->processPath,"-rid",std::to_string(ID),"-l","L2","-action",permit ? "permit" : "deny","-smac",sourceMAC,"-dmac",destMAC);
        pr.wait();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}