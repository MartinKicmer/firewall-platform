//
// Created by andrew on 4/22/26.
//

#pragma once
#include <string>
#include <fstream>
#include "../../firewall-service/headers/DTOS.h"
#include <thread>
#include <csignal>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/bundled/color.h>
#include <netinet/in.h>
#include <arpa/inet.h>
class FileMonitor {
public:
    FileMonitor(std::string filename_) : fileName(std::move(filename_)) {}
    void monitor();
private:
    std::string fileName;

    void printRecord(const CombinedLogRecord& record);
};