#include "../headers/FirewallService.h"



void FirewallService::run(const std::string& standardPath) {
    try {

            this->config = this->loadFromConfig(standardPath);
        std::cout << this->config << std::endl;
        while(1) {
            //std::cout << "Ahoj\n";
        }
    } catch( const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}



std::unique_ptr<FirewallService::Config> FirewallService::loadFromConfig(const std::string& standardPath) const {
    std::array<std::string,1> keywords = {"interface: "};
    std::unique_ptr<FirewallService::Config> loadedConfig = std::make_unique<FirewallService::Config>();
    std::vector<FirewallService::Interface> interfaces;
    std::ifstream ifile(standardPath);
    if(ifile.is_open()) {
        std::string line;
        while(std::getline(ifile,line,'\n')) {
            for(int i = 0; i < 1; i++){
                int firstPos = line.find(keywords[i]);
                if(firstPos != std::string::npos) {
                    if(keywords[i] == "interface: " ) {
                        firstPos = firstPos + keywords[i].length();
                        int lastPos = line.find(',');
                        std::string intName = line.substr(firstPos,lastPos - firstPos);
                        
                        firstPos = line.find(',')+1;
                        std::string state = line.substr(firstPos,line.length()-1);

                        Interface interface = {
                            .name = intName,
                            .state = state
                        };
                        interfaces.push_back(interface);
                    }
                    
                }

            }

        }
        loadedConfig->activeInterfaces = interfaces;
        ifile.close();

        return loadedConfig;
    } else {
        throw std::runtime_error("Could not open config");
    }

}