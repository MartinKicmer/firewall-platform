#pragma  once
#include <tuple>
#include <string>


struct CommonRequest {
    int limitCount;
    int ID;
    bool save;
};

struct EthernetRequest : public CommonRequest {

    std::string source{"none"};
    std::string dest{"none"};
};
struct IPv4Request : public CommonRequest {
    std::tuple<std::string,int> source{"none",-1};
    std::tuple<std::string,int> dest{"none",-1};
    int ttlMax = -1;
    int ttlMin = -1;
    std::string proto;
    int tos = -1;
    bool allowFragments = false;
};

struct L4SimpleRequest : public CommonRequest {
    int sPort = -1;
    int dPort = -1;
};