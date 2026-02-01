#include "../headers/ServerHandler.h"
#include <cstdlib>
#include <memory>
#include <pistache/net.h>
#include <thread>
#include "../headers/WebSocketService.h"
int main(int argc,const char** argv) {
    Pistache::Address addr(Pistache::Ipv4::any(), Pistache::Port(std::atoi(argv[1])));
    auto websocketService = std::make_shared<WebSocketService>(std::atoi(argv[2]),"0.0.0.0");
    auto gateway = std::make_shared<PacketBlockerGateway>("../../firewall/packet-blocker/build/packet-blocker");

    std::thread th(&PacketBlockerGateway::printRedirectedPackets,gateway,websocketService);
    th.detach();
    auto service = std::make_shared<ServerHandler>(websocketService, gateway);
    auto opts = Pistache::Http::Endpoint::options().threads(1);
    Pistache::Http::Endpoint server(addr);
    server.init(opts);
    
    server.setHandler(service->getRouter()->handler()); 
    std::cout << "Server started on port " << argv[1] << " (HTTP) and " << argv[2] << " (WS)" << std::endl;
    server.serve();
}