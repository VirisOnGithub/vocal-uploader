#include "WebServer.h"

#include <httplib.h>
#include <iostream>

namespace vc {

WebServer::WebServer(std::uint16_t port, FileSystem& a_FileSystem, DataBase& a_DataBase) :
	m_FileSystem(a_FileSystem), m_DataBase(a_DataBase) {
	httplib::Server server;

	// test
	server.Get("/hi", [](const httplib::Request& req, httplib::Response& res) {
		std::string page = R"(
			<form method="post" action="/files" enctype="multipart/form-data">
			 <div>
    <label for="say">Quelle salutation voulez-vous dire ?</label>
    <input name="say" id="say" value="Salut" />
  </div>
  <div>
    <label for="file">Choisir un fichier</label>
    <input type="file" id="file" name="audioFile" />
  </div>
  <div>
    <button>Envoyer le fichier</button>
  </div>
</form>
		)";
		res.set_content(page, "text/html");
	});

	// create group
	server.Post("/groups", [](const httplib::Request& req, httplib::Response& res) { res.status = 200; return httplib::Server::HandlerResponse::Handled;});

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
		return httplib::Server::HandlerResponse::Handled;
	});

	// create a file
	server.Post("/files", [this](const httplib::Request& req, httplib::Response& res) {
		
		if (!req.has_file("audioFile")) {
			res.status = 400;
			res.set_content("Il faut préciser un fichier !", "text/plain");
			return httplib::Server::HandlerResponse::Handled;
		}
		
		
		const auto& file = req.get_file_value("audioFile");
		const auto& content = file.content;
		
		std::cout << file.filename << " reçu !" << std::endl;
		
		if (content.size() == 0) {
			res.status = 400;
			res.set_content("Le fichier est vide !", "text/plain");
			return httplib::Server::HandlerResponse::Handled;
		}

		m_FileSystem.WriteFile(file.filename, file.content);
		File dbFile = m_DataBase.CreateFile(file.filename);
		
		res.status = 200;
		res.set_content(dbFile.m_FileUId, "text/plain");
		return httplib::Server::HandlerResponse::Handled;
	});

	// get file info
	server.Get("/files/:id", [](const httplib::Request& req, httplib::Response& res) {
		auto fileId = req.path_params.at("id");
		res.set_content(fileId, "text/plain");
		return httplib::Server::HandlerResponse::Handled;
	});

	// delete file
	server.Delete("/files/:id", [](const httplib::Request& req, httplib::Response& res) {
		auto fileId = req.path_params.at("id");
		res.set_content(fileId, "text/plain");
		return httplib::Server::HandlerResponse::Handled;
	});

	// get file data
	server.set_mount_point("/filesraw", "files");

	server.listen("0.0.0.0", port);
}

}  // namespace vc
