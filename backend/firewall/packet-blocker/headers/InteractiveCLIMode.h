#pragma  once
#include "../../firewall-service/headers/Rules.h"
#include "../../firewall-service/headers/FilterRule.h"
#include <functional>
#include <memory>
#include <tuple>
#include <ncurses.h>
#include <tuple>
#include <vector>
#include <string>
#define USER_INPUT_SIZE 255
class InteractiveCLIMode {
public:
    enum class MenuState { MAIN=0, REDIRECT=1, SELECT=2, REMOVE=3, UPDRATE=4 };
    InteractiveCLIMode() {
        for (const auto& layer : concreteLayerOptions) {
            std::vector<std::string> layerInputs;
            for (size_t i = 0; i < layer.size(); ++i) {
                layerInputs.push_back(""); 
            }
            concreteLayerInputs.push_back(layerInputs);
        }
        this->concreteSelectInputs.resize(this->selectConcreteOptions.size(),"");
        this->concreteRemoveInputs.resize(this->removeConcreteOptions.size(),"");
        this->concreteRedirectInputs.resize(this->redirectConcreteOptions.size(),"");
    }
    void show();
    std::shared_ptr<FilterRule> getParsedFilterRule() { return this->parsedFilterRule; }
private:
    void showLayout();
    void readInput();
    void showLayerOptions();
    void showFrame(int w,int h);
    void showMenuLayout();
    void showMainMenu();
    void showSelectMenu();
    void showRemoveMenu();
    void showRedirectMenu();
    void getSelectionIndex(int min,int max);
    void showBackOption();
    void showConcreteLayerOption();
    void showDoneOption();
    void createFirewallRule();
    void createSelectRule();
    void createRemoveRule();
    void createRedirectRule();
    void showQuitOption();
    void moveIndex(int& index,int ch,int max);
    bool stop = false;
    MenuState currentState;
    int selectionIndex = 0;
    int lastSelectionIndex = 0;
    int ruleIndex = 0;
    char currentUserInput[USER_INPUT_SIZE] = {0};
    std::vector<std::string> layerOptions = {"1. L2 (Ethernet)","2. L3 (IP)","3. L4 (Simple - UDP)","4. L4 (TCP)"};
    std::vector<std::string> selectionOptions = {"1. Firewall rule","2. Redirect rules","3. Select rules","4. Remove rules"};
    std::vector<std::string> selectConcreteOptions = {"1. ID: ","2. From Memory: (y/n)","3. Layer: (LX)","3. Permit: (y/n)"};
    std::vector<std::string> removeConcreteOptions = {"1. ID: ","2. From Memory: (y/n)","3. Layer: (LX)"};
    std::vector<std::string> redirectConcreteOptions = {"1. Permit: ","2. Layer: (LX)","3. Count: (all)"};
    std::vector<std::vector<std::string>> concreteLayerOptions = {
        { "1. Source MAC: ", "2. Dest MAC: ", "3. Permit (y/n): ", "4. Limit: ","5. ID: " }, 
        { "1. Source IP: ", "2. Dest IP: ", "3. TTL Min: ", "4. TTL Max: ", "5. Protocol: ", "6. Permit (y/n): ","5. ID: "  }, 
        { "1. Source Port: ", "2. Dest Port: ", "3. Permit (y/n): ", "4. Limit: ","5. ID: "  }, 
        { "1. S-Port: ", "2. D-Port: ", "3. Flags: ", "4. Win Min: ", "5. Win Max: ", "6. Permit (y/n): ","5. ID: "  } 
    };
    std::vector<std::vector<std::string>> concreteLayerInputs;
    std::vector<std::string> concreteSelectInputs;
    std::vector<std::string> concreteRemoveInputs;
    std::vector<std::string> concreteRedirectInputs;
    std::shared_ptr<FilterRule> parsedFilterRule = nullptr;

    std::function<int(const std::string&,int)> safeStoi = [](const std::string& s, int defaultVal) {
        try { return s.empty() ? defaultVal : std::stoi(s); }
        catch (...) { return defaultVal; }
    };

    std::function<bool(const std::string&)> isPermit = [](const std::string& s) { return s == "y" || s == "Y"; };

    std::function<void(std::tuple<std::string,int>&,const std::string&)> parseIP = [](std::tuple<std::string,int>& ipPref,const std::string& pref) {
        if(pref.empty()) {
             ipPref = std::make_tuple("none",-1);
             return;
        }
        std::stringstream ss(pref);
        std::string line;
        std::string finalIP = "none";
        int finalPref = -1;

        std::getline(ss,line,'/');
        finalIP = line;
        std::getline(ss,line,'/');
        finalPref = std::stoi(line);

        ipPref = std::make_tuple(finalIP,finalPref);
    };

};
