#pragma once
#include <cstdlib>
#include <memory>
#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>
#include "PacketBlockerGateway.h"
#include "RequestTypes.h"
#include "WebSocketService.h"
class ServerHandler : public Pistache::Http::Handler {
public:

    std::shared_ptr<Pistache::Tcp::Handler> clone() const override {
        return std::make_shared<ServerHandler>(this->wsService,this->packetBlockerGateway);
    }
   ServerHandler(std::shared_ptr<WebSocketService> ws_,std::shared_ptr<PacketBlockerGateway> packetBlockerGateway_) 
        : wsService(ws_) {
        this->setupRestRoutes();
        this->packetBlockerGateway = packetBlockerGateway_;
    }
    void onRequest(const Pistache::Http::Request& request, Pistache::Http::ResponseWriter response) override;
    void setupRestRoutes();
    void setupCors(Pistache::Http::ResponseWriter& response);
    void getLastPDUs(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void createRule(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void selectRule(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    void deleteRule(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    std::shared_ptr<Pistache::Rest::Router> getRouter() {
        return std::make_shared<Pistache::Rest::Router>(router);
    }


    EthernetRequest parseEthernetRequest(const Pistache::Rest::Request& request);
    IPv4Request parseIPv4Request(const Pistache::Rest::Request& request);
    L4SimpleRequest parseL4SimpleRequest(const Pistache::Rest::Request& request);
    L4TCPRequest parseL4TCPRequest(const Pistache::Rest::Request& request);

private:
    Pistache::Rest::Router router;
    std::shared_ptr<PacketBlockerGateway> packetBlockerGateway = nullptr;
    std::shared_ptr<WebSocketService> wsService = nullptr;
};