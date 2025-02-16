#include "WebServer.h"

#include <httplib.h>
#include <iostream>
#include <nlohmann/json.hpp>


namespace vc {

static const std::string FILE_EXTENSION = ".ogg";
static const int GROUP_DURATION_DEFAULT = 14;

using json = nlohmann::basic_json<>;


void to_json(json& j, const File& f) {
	j = json{{"uid", f.m_FileUId}, {"name", f.m_FileName}};
	if (!f.m_GroupUId.empty())
		j["groupId"] = f.m_GroupUId;
}

void to_json(json& j, const Group& g) {
	j = json{{"uid", g.m_GroupUId}, {"name", g.m_GroupName}};
}


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
	server.Post("/groups", [this](const httplib::Request& req, httplib::Response& res) {
		if (!req.has_param("name")) {
			res.status = httplib::StatusCode::BadRequest_400;
			res.set_content("You must provide a name", "text/plain");
			return httplib::Server::HandlerResponse::Handled;
		}

		Group group = m_DataBase.CreateGroup(req.get_param_value("name"), GROUP_DURATION_DEFAULT);
		res.set_content(group.m_GroupUId, "plain/text");
		return httplib::Server::HandlerResponse::Handled;
	});

	// get group info
	server.Get("/groups/:id", [this](const httplib::Request& req, httplib::Response& res) {
		auto groupId = req.path_params.at("id");

		auto groupOpt = m_DataBase.GetGroup(groupId);
		if (!groupOpt.has_value()) {
			res.status = httplib::StatusCode::NotFound_404;
			return httplib::Server::HandlerResponse::Handled;
		}

		Group& group = groupOpt.value();
		std::vector<File> files = m_DataBase.GetGroupFiles(group.m_GroupUId);

		json groupJson = group;
		groupJson["files"] = files;

		res.set_content(groupJson.dump(), "application/json");
		return httplib::Server::HandlerResponse::Handled;
	});

	// create a file
	server.Post("/files", [this](const httplib::Request& req, httplib::Response& res) {
		if (!req.has_file("audioFile")) {
			res.status = httplib::StatusCode::BadRequest_400;
			res.set_content("You need to provide file content", "text/plain");
			return httplib::Server::HandlerResponse::Handled;
		}

		const auto& file = req.get_file_value("audioFile");
		const auto& content = file.content;
		std::string fileName = file.filename;

		if (req.has_file("fileName")) {
			fileName = req.get_file_value("fileName").content;
		}

		if (content.size() == 0) {
			res.status = httplib::StatusCode::BadRequest_400;
			res.set_content("The file is empty", "text/plain");
			return httplib::Server::HandlerResponse::Handled;
		}

		std::string groupId = "";
		if (req.has_file("groupId")) {
			groupId = req.get_file_value("groupId").content;
			auto group = m_DataBase.GetGroup(groupId);
			if (!group.has_value()) {
				res.status = httplib::StatusCode::BadRequest_400;
				res.set_content("The group does not exist", "text/plain");
				return httplib::Server::HandlerResponse::Handled;
			}
		}

		File dbFile = m_DataBase.CreateFile(StripExtension(fileName), groupId);
		m_FileSystem.WriteFile(dbFile.m_FileUId + FILE_EXTENSION, file.content);

		res.set_content(dbFile.m_FileUId, "text/plain");
		return httplib::Server::HandlerResponse::Handled;
	});

	// get file info
	server.Get("/files/:id", [this](const httplib::Request& req, httplib::Response& res) {
		auto fileId = req.path_params.at("id");
		std::optional<File> fileOpt = m_DataBase.GetFile(fileId);
		if (!fileOpt.has_value()) {
			res.status = httplib::StatusCode::NotFound_404;
			return httplib::Server::HandlerResponse::Handled;
		}
		File& file = fileOpt.value();
		json jsonFile = file;
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
