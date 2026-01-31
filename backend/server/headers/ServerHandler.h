#pragma once
#include <cstdlib>
#include <memory>
#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>
#include "PacketBlockerGateway.h"
#include "RequestTypes.h"
class ServerHandler : public Pistache::Http::Handler {
public:

    std::shared_ptr<Pistache::Tcp::Handler> clone() const override {
        return std::make_shared<ServerHandler>();
    }
    ServerHandler() {
        this->setupRestRoutes();
        this->packetBlockerGateway = std::make_unique<PacketBlockerGateway>("../../firewall/packet-blocker/build/packet-blocker");
    }
    void onRequest(const Pistache::Http::Request& request, Pistache::Http::ResponseWriter response) override;
    void setupRestRoutes();
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

private:
    Pistache::Rest::Router router;
    std::unique_ptr<PacketBlockerGateway> packetBlockerGateway;
};