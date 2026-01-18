#include "CLIParser.h"

int main(int argc, const char** argv) {
    try {
        CLIParser parser(argc,argv);
        auto filterRule = parser.parseCLIArguments();
        auto rule = filterRule->getRule();
        if(auto derived = std::dynamic_pointer_cast<L2Rule>(rule)) {
            std::cout << *derived << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        std::exit(EXIT_FAILURE);
    }
    
    std::exit(EXIT_SUCCESS);
}
