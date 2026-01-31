#include "../headers/ServerHandler.h"
#include <cstdlib>
#include <pistache/net.h>
int main(int argc,const char** argv) {
    ServerHandler service;
    Pistache::Address addr(Pistache::Ipv4::any(), Pistache::Port(std::atoi(argv[1])));

    auto opts = Pistache::Http::Endpoint::options().threads(1);
    Pistache::Http::Endpoint server(addr);
    server.init(opts);
    
    server.setHandler(service.getRouter()->handler()); 
    
    server.serve();
}