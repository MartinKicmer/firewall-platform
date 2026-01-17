#include "../headers/RawSocket.h"

void RawSocket::initSocket() {

    this->fd = socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
    if(this->fd <= 0) {
        throw std::runtime_error("Could not create RAW SOCKET\n");
    }

    std::get<0>(this->readData) = 0;
    std::memset(std::get<1>(this->readData).data(),0,std::get<1>(this->readData).size());

}

void RawSocket::bindSocket(const std::string& interface) {

    if(fd == -1) {
        throw std::runtime_error("Raw socket wasnt initialized\n");
    }
    int intNumber = if_nametoindex(interface.c_str());
    if(intNumber == 0) {
        throw std::runtime_error("Invalid interface\n");
    }
    struct sockaddr_ll addr;
    std::memset(&addr,0,sizeof(struct sockaddr_ll));

    addr.sll_family = AF_PACKET;
    addr.sll_ifindex = intNumber;
    addr.sll_protocol = htons(ETH_P_ALL);

    int ret = bind(this->fd,(struct sockaddr*)&addr,sizeof(sockaddr_ll));
    if(ret == -1) {
        throw std::runtime_error("Could not bind socket to an interface\n");
    }

    std::cout << "Raw socket bound to interface: " << interface << "\n";
}


void RawSocket::readFromSocket() {
    auto& [bytes, buffer] = this->readData;
    ssize_t recievedBytes = recvfrom(this->fd,buffer.data(),buffer.size(),0,nullptr,nullptr);
    if(recievedBytes < 0) {
        throw std::runtime_error("Error while recieving traffic\n");
    }
    std::get<0>(this->readData) = recievedBytes;


}