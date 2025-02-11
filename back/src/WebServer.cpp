#include "WebServer.h"

#include <httplib.h>

namespace vc {

WebServer::WebServer(std::uint16_t port) {
	httplib::Server server;

	server.Get("/hi", [](const httplib::Request& req, httplib::Response& res) { res.set_content("Hello World!", "text/plain"); });

	server.set_mount_point("/files", "files");

	server.listen("0.0.0.0", 5000);
}

}  // namespace vc
