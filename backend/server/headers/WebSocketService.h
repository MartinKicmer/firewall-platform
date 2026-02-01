#pragma  once
#include <ixwebsocket/IXWebSocketServer.h>
#include <ixwebsocket/IXWebSocket.h>
#include <memory>
#include <thread>
#include <iostream>


class WebSocketService {
public:
    WebSocketService(int port_,const std::string& addr_) {
        this->srv = std::make_unique<ix::WebSocketServer>(port_,addr_);
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