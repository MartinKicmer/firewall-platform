#pragma once
#include <boost/process.hpp>
#include <nlohmann/json.hpp>
#include <boost/process/detail/child_decl.hpp>
#include <boost/process/io.hpp>
#include <boost/process/pipe.hpp>
#include <iostream>
#include "RequestTypes.h"
class PacketBlockerGateway {
public:
    PacketBlockerGateway(const std::string& processPath_) 
    : processPath(processPath_) {}
    nlohmann::json getLastPDUS(int count,const std::string& layer,bool permit);
    void createL2Rule(EthernetRequest req,bool permit);
    void createL3Rule(IPv4Request req,bool permit);
    void createL4simpleRule(L4SimpleRequest req,bool permit);
    nlohmann::json getSelectedRules(int ID,bool permit,const std::string& layer,bool fromMemory);
    void removeRule(int ID,const std::string& layer,bool fromMemory);
private:
    std::string processPath;

};