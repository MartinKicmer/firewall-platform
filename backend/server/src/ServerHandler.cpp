#include "../headers/ServerHandler.h"
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <pistache/http.h>
#include <pistache/http_defs.h>
#include <pistache/router.h>
#include <stdexcept>
#include <string>


L4TCPRequest ServerHandler::parseL4TCPRequest(const Pistache::Rest::Request& request) {

    auto j = nlohmann::json::parse(request.body());
    L4TCPRequest req{};

    if (!j.contains("ID")) throw std::runtime_error("Missing argument ID");

    req.ID = j["ID"].get<int>();
    if (j.contains("limitCount")) req.limitCount = j["limitCount"].get<int>();
    if (j.contains("save"))       req.save = j["save"].get<bool>();
    if (j.contains("sourcePort")) req.sPort = j["sourcePort"].get<int>();
    if (j.contains("destPort"))   req.dPort = j["destPort"].get<int>(); 
    if (j.contains("flags")) req.flags = j["flags"].get<uint8_t>();
    if (j.contains("maxWin")) req.maxWindowSize = j["maxWin"].get<short>();
    if (j.contains("minWin")) req.maxWindowSize = j["minWin"].get<short>();

    return req;
}


void ServerHandler::onRequest(const Pistache::Http::Request& request, Pistache::Http::ResponseWriter response) {
    response.send(Pistache::Http::Code::Ok, "Hello, World\n");
}

void ServerHandler::setupRestRoutes() {
    Pistache::Rest::Routes::Get(this->router, "/fireWall/redirect/:action",Pistache::Rest::Routes::bind(&ServerHandler::getLastPDUs, this));
    Pistache::Rest::Routes::Post(this->router, "/fireWall/createRule/:action",Pistache::Rest::Routes::bind(&ServerHandler::createRule, this));
    Pistache::Rest::Routes::Get(this->router, "/fireWall/selectRule/:action",Pistache::Rest::Routes::bind(&ServerHandler::selectRule, this));
    Pistache::Rest::Routes::Delete(this->router, "/fireWall/deleteRule",Pistache::Rest::Routes::bind(&ServerHandler::deleteRule, this));
    std::cout << "All routes setup\n";
}

void ServerHandler::deleteRule(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    auto query = request.query();
    
    try {
        if(!query.has("ID") || !query.has("layer") || !query.has("fromMemory")) {
            response.send(Pistache::Http::Code::Bad_Request, "Missing arguments");
            return;
        }

        int ID = std::stoi(query.get("ID").value());
        std::string layer = query.get("layer").value();
        bool fromMemory = (query.get("fromMemory").value() == "1");

        this->packetBlockerGateway->removeRule(ID, layer, fromMemory);
        response.send(Pistache::Http::Code::Ok, "Deleted");
    } 
    catch (const std::exception& e) {
        response.send(Pistache::Http::Code::Bad_Request, "Invalid ID format");
        return;
    }
}

EthernetRequest ServerHandler::parseEthernetRequest(const Pistache::Rest::Request& request) {
    auto j = nlohmann::json::parse(request.body());
    EthernetRequest req;

    if (!j.contains("ID")) throw std::runtime_error("Missing argument ID");
    
    req.ID = j["ID"].get<int>();
    if (j.contains("limitCount")) req.limitCount = j["limitCount"].get<int>();
    if (j.contains("save"))       req.save = j["save"].get<bool>(); 
    if (j.contains("sourceMAC"))  req.source = j["sourceMAC"].get<std::string>();
    if (j.contains("destMAC"))    req.dest = j["destMAC"].get<std::string>();

    return req;
}
IPv4Request ServerHandler::parseIPv4Request(const Pistache::Rest::Request& request) {
    auto j = nlohmann::json::parse(request.body());
    IPv4Request req;

    if (!j.contains("ID")) throw std::runtime_error("Missing argument ID");

    req.ID = j["ID"].get<int>();
    if (j.contains("limitCount")) req.limitCount = j["limitCount"].get<int>();
    if (j.contains("save"))       req.save = j["save"].get<bool>();

    if (j.contains("source") && j["source"].is_array()) {
        req.source = std::make_tuple(j["source"][0].get<std::string>(), j["source"][1].get<int>());
    }

    if (j.contains("dest") && j["dest"].is_array()) {
        req.dest = std::make_tuple(j["dest"][0].get<std::string>(), j["dest"][1].get<int>());
    }

    if (j.contains("ttlMax"))    req.ttlMax = j["ttlMax"].get<int>();
    if (j.contains("ttlMin"))    req.ttlMin = j["ttlMin"].get<int>();
    if (j.contains("protocol"))  req.proto = j["protocol"].get<std::string>();
    if (j.contains("tos"))       req.tos = j["tos"].get<int>();
    if (j.contains("allowFrag")) req.allowFragments = j["allowFrag"].get<bool>();

    return req;
}
L4SimpleRequest ServerHandler::parseL4SimpleRequest(const Pistache::Rest::Request& request) {
    auto j = nlohmann::json::parse(request.body());
    L4SimpleRequest req;

    if (!j.contains("ID")) throw std::runtime_error("Missing argument ID");

    req.ID = j["ID"].get<int>();
    if (j.contains("limitCount")) req.limitCount = j["limitCount"].get<int>();
    if (j.contains("save"))       req.save = j["save"].get<bool>();
    if (j.contains("sourcePort")) req.sPort = j["sourcePort"].get<int>();
    if (j.contains("destPort"))   req.dPort = j["destPort"].get<int>(); 

    return req;
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
    std::string layer = j["layer"].get<std::string>();
    try {
        if(layer == "L2") {
         EthernetRequest req = this->parseEthernetRequest(request);
         this->packetBlockerGateway->createL2Rule(req,permit);
        }
        if(layer == "L3") {
            IPv4Request req = this->parseIPv4Request(request);
            this->packetBlockerGateway->createL3Rule(req, permit);
        }
        if(layer == "L4Simple") {
            L4SimpleRequest req = this->parseL4SimpleRequest(request);
            this->packetBlockerGateway->createL4simpleRule(req,permit);
        }
        if(layer == "L4TCP") {
            L4TCPRequest req = this->parseL4TCPRequest(request);
            std::cout << "TCP request parsed\n";
            this->packetBlockerGateway->createL4TCPRule(req, permit);
        }
     } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        response.send(Pistache::Http::Code::Bad_Request);
        std::exit(EXIT_FAILURE);
    }
   
    response.send(Pistache::Http::Code::Ok);
}

void ServerHandler::getLastPDUs(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    try {
        bool permit = request.param(":action").as<bool>();
        auto query = request.query();
        if(!query.has("count") || !query.has("layer")) response.send(Pistache::Http::Code::Bad_Request);

        int count = std::stoi(query.get("count").value());
        std::string layer = query.get("layer").value();
        
        this->packetBlockerGateway->setStreamParams(layer,permit);
        response.send(Pistache::Http::Code::Ok);
    } catch (const std::exception& e) {
        response.send(Pistache::Http::Code::Internal_Server_Error);
        std::exit(EXIT_FAILURE);
    } 
}
