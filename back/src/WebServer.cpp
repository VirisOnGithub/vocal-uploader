#include "WebServer.h"

#include <httplib.h>
#include <iostream>
#include <nlohmann/json.hpp>

namespace vc {

static const std::string FILE_EXTENSION = ".ogg";

using json = nlohmann::basic_json<>;

static std::string StripExtension(const std::string& fileName) {
	std::size_t index = fileName.find(".");
	if (index == std::string::npos)
		return fileName;
	return fileName.substr(0, index);
}

WebServer::WebServer(std::uint16_t port, FileSystem& a_FileSystem, DataBase& a_DataBase) :
	m_FileSystem(a_FileSystem), m_DataBase(a_DataBase) {
	httplib::Server server;

	// create group
	server.Post("/groups", [](const httplib::Request& req, httplib::Response& res) {
		res.status = 200;
		return httplib::Server::HandlerResponse::Handled;
	});

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
		return httplib::Server::HandlerResponse::Handled;
	});

	// create a file
	server.Post("/files", [this](const httplib::Request& req, httplib::Response& res) {
		if (!req.has_file("audioFile")) {
			res.status = 400;
			res.set_content("You need to provide file content", "text/plain");
			return httplib::Server::HandlerResponse::Handled;
		}

		const auto& file = req.get_file_value("audioFile");
		const auto& content = file.content;

		if (content.size() == 0) {
			res.status = 400;
			res.set_content("The file is empty", "text/plain");
			return httplib::Server::HandlerResponse::Handled;
		}

		File dbFile = m_DataBase.CreateFile(StripExtension(file.filename));
		m_FileSystem.WriteFile(dbFile.m_FileUId + FILE_EXTENSION, file.content);

		res.set_content(dbFile.m_FileUId, "text/plain");
		return httplib::Server::HandlerResponse::Handled;
	});

	// get file info
	server.Get("/files/:id", [this](const httplib::Request& req, httplib::Response& res) {
		auto fileId = req.path_params.at("id");
		std::optional<File> fileOpt = m_DataBase.GetFile(fileId);
		if (!fileOpt.has_value()) {
			res.status = 404;
			return httplib::Server::HandlerResponse::Handled;
		}
		File& file = fileOpt.value();
		json jsonFile = {
			{"uid", file.m_FileUId},
			{"name", file.m_FileName},
		};
		if (!file.m_GroupUId.empty())
			jsonFile["groupId"] = file.m_GroupUId;
		res.set_content(jsonFile.dump(), "application/json");
		return httplib::Server::HandlerResponse::Handled;
	});

	// dirty tricks
	server.set_post_routing_handler([](const httplib::Request& req, httplib::Response& res) {
		// prevent CORS
		if (req.has_header("Origin")) {
			res.set_header("Access-Control-Allow-Origin", req.get_header_value("Origin"));
		}
		// redirect to ogg
		if (req.path.find("filesraw") != std::string::npos && req.path.find('.') == std::string::npos) {
			res.set_redirect(req.path + FILE_EXTENSION);
		}
	});

	// get file data
	server.set_mount_point("/filesraw", "files");

	server.listen("0.0.0.0", port);
}

}  // namespace vc
