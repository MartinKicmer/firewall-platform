#include <mqueue.h>
#include <string>
#include <stdexcept>
#include <optional>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <cstdlib>
class MQConnector {
public:
    MQConnector(const std::string& path_) 
    : mqDesc(-1),path(path_),BUFFER{0} {
        std::memset(&this->attr,0,sizeof(struct mq_attr));
    }

    ~MQConnector() {
        mq_close(this->mqDesc);
    }

    std::tuple<bool,std::string> recieveData();
    void sendData(const std::string& data);
    void create();
    void connect();

    void close() { 
        if(this->mqDesc > 0) {
            mq_close(this->mqDesc);
        }
    }
private:
    struct mq_attr attr;
    mqd_t mqDesc;
    std::string path;
    char BUFFER[8192];
};