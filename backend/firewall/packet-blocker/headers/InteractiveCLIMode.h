#pragma  once
#include "../../firewall-service/headers/Rules.h"
#include "../../firewall-service/headers/FilterRule.h"
#include <memory>
#include <tuple>
#include <ncurses.h>
#include <tuple>
#include <vector>
#include <string>
#define USER_INPUT_SIZE 255
class InteractiveCLIMode {
public:
    enum class MenuState { MAIN=0, L2_FORM=1, L3_FORM=2, L4_SIMPLE_FORM=3, L4_TCP_FORM=4 };
    InteractiveCLIMode() {
        for (const auto& layer : concreteLayerOptions) {
        std::vector<std::string> layerInputs;
        for (size_t i = 0; i < layer.size(); ++i) {
            layerInputs.push_back(""); 
        }
        concreteLayerInputs.push_back(layerInputs);
    }
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
    void getSelectionIndex(int min,int max);
    void showBackOption();
    void showConcreteLayerOption();
    void showDoneOption();
    void createFirewallRule();
    void showQuitOption();
    bool stop = false;
    MenuState currentState;
    int selectionIndex = 0;
    int lastSelectionIndex = 0;
    int ruleIndex = 0;
    char currentUserInput[USER_INPUT_SIZE] = {0};
    std::vector<std::string> layerOptions = {"1. L2 (Ethernet)","2. L3 (IP)","3. L4 (Simple - UDP)","4. L4 (TCP)"};
    std::vector<std::string> selectionOptions = {"1. Firewall rule","2. Redirect rules","3. Select rules","4. Remove rules"};

    std::vector<std::vector<std::string>> concreteLayerOptions = {
        { "1. Source MAC: ", "2. Dest MAC: ", "3. Permit (y/n): ", "4. Limit: ","5. ID: " }, 
        { "1. Source IP: ", "2. Dest IP: ", "3. TTL Min: ", "4. TTL Max: ", "5. Protocol: ", "6. Permit (y/n): ","5. ID: "  }, 
        { "1. Source Port: ", "2. Dest Port: ", "3. Permit (y/n): ", "4. Limit: ","5. ID: "  }, 
        { "1. S-Port: ", "2. D-Port: ", "3. Flags: ", "4. Win Min: ", "5. Win Max: ", "6. Permit (y/n): ","5. ID: "  } 
    };
    std::vector<std::vector<std::string>> concreteLayerInputs;

    std::shared_ptr<FilterRule> parsedFilterRule = nullptr;
};
