#include "../headers/MQConnector.h"
#include <cstring>

void MQConnector::create() {
    this->attr.mq_flags = 0;
    this->attr.mq_maxmsg = 10;     
    this->attr.mq_msgsize = 8192;  
    this->attr.mq_curmsgs = 0;
    const char* currentPath = this->path.c_str();
    mq_unlink(currentPath);
    mode_t oldMask = umask(0);
    this->mqDesc = mq_open(currentPath, O_RDONLY | O_CREAT, 0777, &attr);
    umask(oldMask);
    if(this->mqDesc == static_cast<mqd_t>(-1)) {
        throw std::runtime_error("Could not create message queuen\n");
    }
}

void MQConnector::connect() {
    const char* currentPath = this->path.c_str();
    this->mqDesc = mq_open(currentPath,O_WRONLY);
      if(this->mqDesc == static_cast<mqd_t>(-1)) {
        throw std::runtime_error("Could not connect to a message queuen\n");
    }
}

std::tuple<bool, std::string> MQConnector::recieveData() {
    ssize_t bytes = mq_receive(this->mqDesc, this->BUFFER, 8192, nullptr);
    if (bytes < 0) {
        throw std::runtime_error("Error while receiving from message queue");
    }
    std::string dataStr(this->BUFFER, static_cast<size_t>(bytes));
    std::memset(this->BUFFER, 0, 8192);
    bool isEnd = (dataStr == "end");
    return std::make_tuple(isEnd, dataStr);
}

void MQConnector::sendData(const std::string& data) {

    int ret = mq_send(this->mqDesc,data.c_str(),data.length(),0);
    if(ret == -1) {
        throw std::runtime_error("Error while sending data to message queue\n");
    }

}



