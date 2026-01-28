#include <boost/process/detail/child_decl.hpp>
#include <boost/process/io.hpp>
#include <boost/process/pipe.hpp>
#include <cstdlib>
#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>
#include <boost/process.hpp>
#include <nlohmann/json.hpp>
class ServerHandler : public Pistache::Http::Handler {
public:

    HTTP_PROTOTYPE(ServerHandler)

    ServerHandler() {
        this->setupRestRoutes();
    }
    void onRequest(const Pistache::Http::Request& request, Pistache::Http::ResponseWriter response) override {
        response.send(Pistache::Http::Code::Ok, "Hello, World\n");
    }

    void setupRestRoutes() {
        Pistache::Rest::Routes::Get(this->router, "/value/:count",Pistache::Rest::Routes::bind(&ServerHandler::getLastPDUs, this));
        std::cout << "All routes setup\n";
    }

    void getLastPDUs(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
        std::string noPDUs = request.param(":count").as<std::string>();
        boost::process::ipstream outStream;
        //-l L3 -action deny redirect 10
        nlohmann::json jsonArr = nlohmann::json::array();
        try {
            boost::process::child pr("../../firewall/packet-blocker/build/packet-blocker",
                "-l","L3","-action","deny","redirect",noPDUs.c_str(),boost::process::std_out > outStream);

            std::string line;
            while (std::getline(outStream, line) && !line.empty()) {
                if(line.empty()) continue;
                try {
                    jsonArr.push_back(nlohmann::json::parse(line));
                } catch(...) {
                    continue;
                }
            }

            pr.wait();

            response.headers().add<Pistache::Http::Header::ContentType>(MIME(Application, Json));
            response.send(Pistache::Http::Code::Ok, jsonArr.dump());
            
        } catch (const std::exception& e) {
            std::cerr << "Eror while trying to capture last PDUS: " << e.what() << std::endl;
            std::exit(EXIT_FAILURE);
        }

    }

    std::shared_ptr<Pistache::Rest::Router> getRouter() {
        return std::make_shared<Pistache::Rest::Router>(router);
    }

private:
    Pistache::Rest::Router router;
};