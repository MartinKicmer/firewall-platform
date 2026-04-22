//
// Created by andrew on 4/22/26.
//
#include  "../headers/FileMonitor.h"


void debug_handler(int signum) {
    spdlog::warn("Debug session ended");
    exit(signum);
}

std::string macToString(const uint8_t mac[6]) {
    return fmt::format("{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}",
                       mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

std::string ipToString(uint32_t ip) {
    struct in_addr addr;
    addr.s_addr = ip;
    return inet_ntoa(addr);
}

void FileMonitor::monitor() {
    spdlog::set_pattern("%^[%H:%M:%S] %v%$");
    spdlog::info("Starting Firewall Monitor on file: {}", this->fileName);
    spdlog::info("{:<8} | {:<15} | {:<15} | {:<6} | {:<10}", "VERDICT", "SOURCE", "DEST", "PROTO", "INFO");
    spdlog::info(std::string(70, '-'));
    std::signal(SIGINT, debug_handler);
    std::ifstream ifs(this->fileName,std::ios::binary);
    if (!ifs.is_open()) {
        throw std::runtime_error("Cannot open file for debug");
    }
    CombinedLogRecord record{};
    while (true) {
        if (ifs.read(reinterpret_cast<char*>(&record), sizeof(CombinedLogRecord))) {
            this->printRecord(record);
        } else {
            ifs.clear();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void FileMonitor::printRecord(const CombinedLogRecord& record) {
    std::string verdict = (record.verdict == 1) ? "ALLOW" : "BLOCK";
    std::string src = record.hasl3 ? ipToString(record.ip.srcIp) : "N/A";
    std::string dst = record.hasl3 ? ipToString(record.ip.destIp) : "N/A";

    std::string proto = "UNKNOWN";
    std::string extra = "";

    if (record.l4Type == L4Type::TCP) {
        proto = "TCP";
        extra = fmt::format("Port: {}", ntohs(record.l4.tcp.dPort));
    } else if (record.l4Type == L4Type::UDP) {
        proto = "UDP";
        extra = fmt::format("Port: {}", ntohs(record.l4.udp.dPort));
    }

    if (record.verdict == 1) {
        spdlog::info("{:<8} | {:<15} | {:<15} | {:<6} | {:<10}",
                     verdict, src, dst, proto, extra);
        if (record.hasl2) {
            auto formatted = fmt::format(fg(fmt::color::dodger_blue), "[L2] MAC: {:<17} -> {:<17} | Type: 0x{:04x}",
                                         macToString(record.eth.srcMac),
                                         macToString(record.eth.destMac),
                                         record.eth.etherType);
            spdlog::info("\t{}", formatted);
        }

        if (record.hasl3) {
            auto formatted = fmt::format(fg(fmt::color::cyan), "[L3] TTL: {:<3} | TOS: 0x{:02x} | ID: {}",
                                         record.ip.ttl,
                                         record.ip.service_type,
                                         record.ip.identification);
            spdlog::info("\t{}", formatted);
        }

        if (record.l4Type == L4Type::TCP) {
            auto formatted = fmt::format(fg(fmt::color::gold), "[L4] TCP Seq: {} | Win: {} | SPort: {}",
                                         record.l4.tcp.seqNumber,
                                         record.l4.tcp.windowSize,
                                         ntohs(record.l4.tcp.sPort));
            spdlog::info("\t{}", formatted);
        }
        else if (record.l4Type == L4Type::UDP) {
            auto formatted = fmt::format(fg(fmt::color::gold), "[L4] UDP Len: {} | SPort: {}",
                                         record.l4.udp.len,
                                         ntohs(record.l4.udp.sPort));
            spdlog::info("\t{}", formatted);
        }
    } else {
        spdlog::error("{:<8} | {:<15} | {:<15} | {:<6} | {:<10}",
                      verdict, src, dst, proto, extra);
    }
}
