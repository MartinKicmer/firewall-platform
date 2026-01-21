#include <mqueue.h>
#include <string>
#include <stdexcept>
#include <optional>
#include <sys/types.h>
#include <sys/stat.h>
class MQConnector {
public:
    MQConnector(const std::string& path) 
    : attr{0},mqDesc(-1),path(path),BUFFER{0} {}

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