#include <cstdlib>
#include <iostream>
#include "FirewallService.h"
int main(void) {

    FirewallService service;
    service.run("../firewall_config.txt");

    std::exit(EXIT_SUCCESS);
}