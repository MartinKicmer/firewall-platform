#include "../headers/ServerHandler.h"
#include <pistache/http_defs.h>
#include <string>


void ServerHandler::onRequest(const Pistache::Http::Request& request, Pistache::Http::ResponseWriter response) {
    response.send(Pistache::Http::Code::Ok, "Hello, World\n");
}

void ServerHandler::setupRestRoutes() {
    Pistache::Rest::Routes::Get(this->router, "/fireWall/redirect/:action",Pistache::Rest::Routes::bind(&ServerHandler::getLastPDUs, this));
    Pistache::Rest::Routes::Post(this->router, "/fireWall/createRule/:action",Pistache::Rest::Routes::bind(&ServerHandler::createRule, this));
    Pistache::Rest::Routes::Get(this->router, "/fireWall/selectRule/:action",Pistache::Rest::Routes::bind(&ServerHandler::selectRule, this));
    std::cout << "All routes setup\n";
}

void ServerHandler::selectRule(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    std::string permitStr = request.param(":action").as<std::string>();
    bool permit = (permitStr == "1" || permitStr == "true");
    auto query = request.query();

    if(!query.has("ID") || !query.has("layer") || !query.has("fromMemory") ) {
        response.send(Pistache::Http::Code::Bad_Request,"Missing arguments layer or ID or fromMemory");
        return;
    }
    int ID = std::stoi(query.get("ID").value());
    std::string layer = query.get("layer").value();
    bool fromMemory = std::stoi(query.get("fromMemory").value());
    nlohmann::json ruleArr = this->packetBlockerGateway->getSelectedRules(ID, permit, layer, fromMemory);

    response.headers().add<Pistache::Http::Header::ContentType>(MIME(Application, Json));
    response.send(Pistache::Http::Code::Ok, ruleArr.dump());
    
}

void ServerHandler::createRule(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    std::string permitStr = request.param(":action").as<std::string>();
    bool permit = (permitStr == "1" || permitStr == "true");
    std::string body = request.body();
    if(body.empty()) {
        response.send(Pistache::Http::Code::Bad_Request,"Missing arguments for creating a rule\n");
        return;
    }
    auto j = nlohmann::json::parse(body);
    if(!j.contains("layer")) {
        response.send(Pistache::Http::Code::Bad_Request,"Missing argument layer\n");
        return;
    }
    if(!j.contains("ID")) {
        response.send(Pistache::Http::Code::Bad_Request,"Missing argument ID\n");
        return;
    }
    std::string layer = j["layer"];
    int ID = j["ID"];
    std::string sourceMAC = j["sourceMAC"];
    std::string destMAC = j["destMAC"];
    this->packetBlockerGateway->createL2Rule(ID, permit, sourceMAC, destMAC);
    response.send(Pistache::Http::Code::Ok);
}

void ServerHandler::getLastPDUs(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    bool permit = request.param(":action").as<bool>();
    auto query = request.query();
    if(!query.has("count") || !query.has("layer")) response.send(Pistache::Http::Code::Bad_Request);

    int count = std::stoi(query.get("count").value());
    std::string layer = query.get("layer").value();
    
    auto jsonArr = this->packetBlockerGateway->getLastPDUS(count,layer,permit);

    response.headers().add<Pistache::Http::Header::ContentType>(MIME(Application, Json));
    response.send(Pistache::Http::Code::Ok, jsonArr.dump());
        
      
}
