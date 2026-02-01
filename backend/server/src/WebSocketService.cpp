#include "../headers/WebSocketService.h"

void WebSocketService::setupWSThread() {
    this->wsThread = std::thread([this](){
        this->srv->start();
    });
    this->wsThread.detach();
}

void WebSocketService::broadcastMSG(const std::string& msg) {
    for(const auto& cl : this->srv->getClients()) {
        cl->send(msg);
    } 
}

void WebSocketService::socketCallback(std::shared_ptr<ix::ConnectionState> connectionState, 
                                        ix::WebSocket& webSocket, 
                                        const ix::WebSocketMessagePtr& msg) {

    if (msg->type == ix::WebSocketMessageType::Open) {
        std::cout << "New connection: " << connectionState->getId() << std::endl;
        webSocket.send("Full packet stream from firewall"); 
    } 
    else if (msg->type == ix::WebSocketMessageType::Close) {
        std::cout << "WebSocket Client left: " << connectionState->getId() << std::endl;
    }
}