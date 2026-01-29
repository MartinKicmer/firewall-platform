#pragma once
#include <boost/process.hpp>
#include <nlohmann/json.hpp>
#include <boost/process/detail/child_decl.hpp>
#include <boost/process/io.hpp>
#include <boost/process/pipe.hpp>
#include <iostream>
class PacketBlockerGateway {
public:
    PacketBlockerGateway(const std::string& processPath_) 
    : processPath(processPath_) {}
    nlohmann::json getLastPDUS(int count,const std::string& layer,bool permit);

private:
    std::string processPath;

};