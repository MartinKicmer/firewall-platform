#include <string>
#include <vector>
#include <thread>
#include <cstring>
#include <fstream>
#include <array>
#include <sstream>
#include <iostream>
class FirewallService {
public:
    struct Interface {
        std::string name;
        std::string state;
    };
    struct Config {
        std::vector<Interface> activeInterfaces;

        friend std::ostream& operator<<(std::ostream& o, const std::unique_ptr<Config>& config) {
            o << "Running Config\n";
            o << "Active interfaces\n";
            for(const auto& i : config->activeInterfaces) {
                o << "interface: " << i.name << " state: " << i.state << std::endl;
            }
            return o;

        }
    };
    FirewallService() : config(nullptr),socketDescriptor(-1) {
        std::memset(BUFFER,0,BUFSIZ * sizeof(uint8_t));
    }
    void run(const std::string& standardPath);
private:
    [[nodiscard]] std::unique_ptr<FirewallService::Config> loadFromConfig(const std::string& standardPath) const;
    void createEthernetSocket();
    void packetBlockerCommunicator();    

    
    std::unique_ptr<Config> config;
    std::thread packetBlockerT;
    int socketDescriptor;
    uint8_t BUFFER[BUFSIZ];

};