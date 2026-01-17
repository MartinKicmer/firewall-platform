#pragma once
#include <string>
#include <stdexcept>   
#include <cstring>     

#include <sys/socket.h>    
#include <sys/ioctl.h>     
#include <netpacket/packet.h> 
#include <net/ethernet.h>  
#include <net/if.h>        
#include <arpa/inet.h>    
#include <unistd.h>        
#include <iostream>
#include <array>
class RawSocket {
public:
    RawSocket() : fd(-1) {}
    ~RawSocket() {
        if(this->fd > 0) {
            close(this->fd);
        }
    }
    void bindSocket(const std::string& interface);
    void initSocket();
    void readFromSocket();
    [[nodiscard]] std::tuple<ssize_t, std::array<uint8_t, BUFSIZ>>& getReadData() { return this->readData; }
private:
    int fd;
    std::tuple<ssize_t, std::array<uint8_t, BUFSIZ>> readData;

};