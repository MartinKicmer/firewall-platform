#include <string>

class PacketBlocker {
public:
    PacketBlocker() {}
    void parseCLIArgs(int argc,char** argv);
    void sentXMLOutputToService();
private:
    std::string xmlOutput;

};