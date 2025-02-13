#include <iostream>

#include "DataBase.h"
#include "FileSystem.h"
#include "WebServer.h"

int main(int argc, char** argv) {

	static const std::string host = "local_db";

	vc::DataBase database{"tcp://" + host + ":3306", "root", "super_strong_password", "vcupload"};

	vc::FileSystem filesystem {"files"};

	static const std::uint16_t port = 5000;

	std::cout << "Server running on port " << port << std::endl;

	vc::WebServer server {port, filesystem, database};

	std::cout << "Server stopped !" << std::endl;

	return 0;
}
