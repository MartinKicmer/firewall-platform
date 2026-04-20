#pragma  once
#include <cstddef>
#include <libnetfilter_queue/libnetfilter_queue.h>
#include "FirewallService.h"
#include <tuple>


class KernelSocket {
public:
    KernelSocket(FirewallService* service_,int queueNum_) : service(service_),fd(-1),queueNum(queueNum_) {
        this->init();
    }
    static const std::size_t BUFFERSIZE  = 65536;
    void recieveData();  
    void run();   
private:
    void init();
    void setCallback();

    FirewallService* service;
    int fd;
    struct nfq_handle* handle = nullptr;
    struct nfq_q_handle* queueHandle = nullptr;
    int queueNum = 0;

};