#include "../headers/KernelSocket.h"
#include <array>
#include <cstdint>
#include <cstdio>
#include <libnetfilter_queue/libnetfilter_queue.h>
#include <stdexcept>
#include <sys/socket.h>
#include <tuple>


void KernelSocket::init() {
    this->handle = nfq_open();
    if(!this->handle) throw std::runtime_error("Could not open NFQ Handle\n");

    if(nfq_bind_pf(this->handle, AF_BRIDGE) < 0) throw std::runtime_error("Could not start getting L2 packets from Kernel\n");

    this->setCallback();
    this->fd = nfq_fd(this->handle);

    if(fd < 0) throw std::runtime_error("Invalid kernel file descriptor\n");

    std::cout << "Kernel socket initialized with fd: " << this->fd << std::endl;
}
void KernelSocket::setCallback() {
    this->queueHandle = nfq_create_queue(
        this->handle, 
        0,                                      
        &FirewallService::handlePacketCallback, 
        static_cast<void*>(this->service)
    );

    if (!this->queueHandle) {
        throw std::runtime_error("Could not create nfq queue 0\n");
    }

    if (nfq_set_mode(this->queueHandle, NFQNL_COPY_PACKET, 0xffff) < 0) {
        throw std::runtime_error("Can't set packet copy mode\n");
    }

    std::cout << "Firewall callback set and queue mode configured\n";
}


void KernelSocket::recieveData() {
    std::array<char,KernelSocket::BUFFERSIZE> BUFFER{};
    ssize_t bytes = recv(this->fd, BUFFER.data(), BUFFER.size(), 0);
    if (bytes < 0) {
       throw std::runtime_error("Error while recieving data from kernel\n");
    }
    nfq_handle_packet(this->handle, BUFFER.data(), bytes);
}
