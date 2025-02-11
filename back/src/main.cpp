#include <iostream>

#include "DataBase.h"
#include "FileSystem.h"
#include "WebServer.h"

int main(int argc, char** argv) {

	vc::DataBase database{"tcp://local_db:3306", "root", "super_strong_password", "vcupload"};

	vc::FileSystem filesystem {"files"};

	static const std::uint16_t port = 5000;

	std::cout << "Server running on port " << port << " !\n";

	vc::WebServer server {port};

	return 0;
}
