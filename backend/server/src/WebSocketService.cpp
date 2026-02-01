#include "../headers/WebSocketService.h"

void WebSocketService::setupWSThread() {
    auto serverPtr = this->srv; 
    
    this->wsThread = std::thread([serverPtr]() {
        serverPtr->start();
    });
    
    this->wsThread.detach();
}

void WebSocketService::broadcastMSG(const std::string& msg) {
    if (!this->srv) return;

    auto clients = this->srv->getClients();
    if (clients.empty()) {
        return; 
    }

    for(const auto& cl : clients) {
        cl->send(msg);
    }
}

void WebSocketService::socketCallback(std::shared_ptr<ix::ConnectionState> connectionState, 
                                        ix::WebSocket& webSocket, 
                                        const ix::WebSocketMessagePtr& msg) {

    std::cout << "DEBUG: Přijat event typu: " << static_cast<int>(msg->type) << std::endl;

    if (msg->type == ix::WebSocketMessageType::Open) {
        std::cout << "New connection: " << connectionState->getId() << std::endl;
        webSocket.send("Full packet stream from firewall"); 
    }
    else if (msg->type == ix::WebSocketMessageType::Close) {
        std::cout << "WebSocket Client left: " << connectionState->getId() << std::endl;
    }
}