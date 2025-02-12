#include "WebServer.h"

#include <httplib.h>

namespace vc {

WebServer::WebServer(std::uint16_t port) {
	httplib::Server server;

	// test
	server.Get("/hi", [](const httplib::Request& req, httplib::Response& res) { res.set_content("Hello World!", "text/plain"); });

	// create group
	server.Post("/groups", [](const httplib::Request& req, httplib::Response& res) { res.status = 200; });
	
	// get group info
	server.Get("/groups/:id", [](const httplib::Request& req, httplib::Response& res) { 
		auto groupIdString = req.path_params.at("id");
		int groupId;
		try {
			groupId = std::stoi(groupIdString);
		} catch (std::invalid_argument& e) {
			res.status = 400;
			return httplib::Server::HandlerResponse::Handled;
		}
		res.set_content(std::to_string(groupId), "text/plain");
	});

	// delete a group
	server.Delete("/groups/:id", [](const httplib::Request& req, httplib::Response& res) { 
		auto groupIdString = req.path_params.at("id");
		int groupId;
		try {
			groupId = std::stoi(groupIdString);
		} catch (std::invalid_argument& e) {
			res.status = 400;
			return httplib::Server::HandlerResponse::Handled;
		}
		res.set_content(std::to_string(groupId), "text/plain");
	});

	// create a file
	server.Post("/files", [](const httplib::Request& req, httplib::Response& res) { res.status = 200; });

	// get file info
	server.Get("/files/:id", [](const httplib::Request& req, httplib::Response& res) { 
		auto fileId = req.path_params.at("id");
		res.set_content(fileId, "text/plain");
	});

	// delete file
	server.Delete("/files/:id", [](const httplib::Request& req, httplib::Response& res) { 
		auto fileId = req.path_params.at("id");
		res.set_content(fileId, "text/plain");
	});

	// get file data
	server.set_mount_point("/filesraw", "files");

	server.listen("0.0.0.0", port);
}

}  // namespace vc
