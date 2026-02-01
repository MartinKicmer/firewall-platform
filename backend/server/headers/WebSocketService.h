#pragma  once
#include <ixwebsocket/IXWebSocketServer.h>
#include <ixwebsocket/IXWebSocket.h>
#include <memory>
#include <thread>
#include <iostream>


class WebSocketService {
public:
    WebSocketService(int port_,const std::string& addr_) {
        this->srv = std::make_shared<ix::WebSocketServer>(port_,addr_);
       this->srv->setOnClientMessageCallback(
        [this](std::shared_ptr<ix::ConnectionState> connectionState, 
               ix::WebSocket& webSocket, 
               const ix::WebSocketMessagePtr& msg) {
                    this->socketCallback(connectionState, webSocket, msg);
            }
        );
        auto res = this->srv->listen();
        if (!res.first) {
            std::cerr << "WebSocket Error: Port " << port_ << " Could not open " << res.second << std::endl;
            return;
        }
        this->setupWSThread();
    }
    void socketCallback(std::shared_ptr<ix::ConnectionState> connectionState, 
                                        ix::WebSocket& webSocket, 
                                        const ix::WebSocketMessagePtr& msg);
    void setupWSThread();
    void broadcastMSG(const std::string& msg);
private:
   std::shared_ptr<ix::WebSocketServer> srv = nullptr;
   std::thread wsThread;
};