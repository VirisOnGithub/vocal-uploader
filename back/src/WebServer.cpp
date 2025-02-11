#include "WebServer.h"

#include <httplib.h>

namespace vc {

WebServer::WebServer(std::uint16_t port) {
	httplib::Server server;

	server.Get("/hi", [](const httplib::Request& req, httplib::Response& res) { res.set_content("Hello World!", "text/plain"); });

	server.listen("127.0.0.1", 5000);
}

}  // namespace vc
