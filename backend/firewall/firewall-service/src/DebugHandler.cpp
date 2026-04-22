//
// Created by andrew on 4/22/26.
//

#include "../headers/DebugHandler.h"

#include "FirewallService.h"
void DebugHandler::handleLogging(FirewallService* firewallService) {
     std::ofstream ofs(this->fileName, std::ios::binary | std::ios::app);

     try {
          if (!ofs.is_open()) {
               throw std::runtime_error("Could not open DEBUG file for writing: " + this->fileName);
          }
          while (!FirewallService::STOP_DEBUG()) {
               auto record = firewallService->getLogBuffer().read();
               ofs.write(reinterpret_cast<const char*>(&record), sizeof(CombinedLogRecord));
          }


     } catch (const std::exception& e) {
          std::cerr << "Logging Error: " << e.what() << std::endl;
     }

     if (ofs.is_open()) {
          ofs.close();
          std::ofstream truncate_stream(this->fileName, std::ios::trunc);
          truncate_stream.close();

          std::cout << "Exiting firewall debug session and clearing log file.\n";
          std::cout << "Exiting firewall debug session\n";
          std::exit(EXIT_SUCCESS);
     }
}
