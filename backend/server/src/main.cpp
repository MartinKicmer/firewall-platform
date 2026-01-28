#include "../headers/ServerHandler.h"
#include <cstdlib>
#include <pistache/net.h>
int main() {
    ServerHandler service;
    Pistache::Address addr(Pistache::Ipv4::any(), Pistache::Port(9080));

    auto opts = Pistache::Http::Endpoint::options().threads(1);
    Pistache::Http::Endpoint server(addr);
    server.init(opts);
    
    server.setHandler(service.getRouter()->handler()); 
    
    server.serve();
}