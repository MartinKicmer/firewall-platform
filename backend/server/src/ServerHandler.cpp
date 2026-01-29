#include "../headers/ServerHandler.h"
#include <pistache/http_defs.h>
#include <string>


void ServerHandler::onRequest(const Pistache::Http::Request& request, Pistache::Http::ResponseWriter response) {
    response.send(Pistache::Http::Code::Ok, "Hello, World\n");
}

void ServerHandler::setupRestRoutes() {
    Pistache::Rest::Routes::Get(this->router, "/fireWall/redirect/:action",Pistache::Rest::Routes::bind(&ServerHandler::getLastPDUs, this));
    std::cout << "All routes setup\n";
}

void ServerHandler::getLastPDUs(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    bool permit = request.param(":action").as<bool>();
    auto query = request.query();
    if(!query.has("count") || !query.has("layer")) response.send(Pistache::Http::Code::No_Content);

    int count = std::stoi(query.get("count").value());
    std::string layer = query.get("layer").value();
    
    auto jsonArr = this->packetBlockerGateway->getLastPDUS(count,layer,permit);

    response.headers().add<Pistache::Http::Header::ContentType>(MIME(Application, Json));
    response.send(Pistache::Http::Code::Ok, jsonArr.dump());
        
      
}
