#include <iostream>

#include "DataBase.h"

#include "WebServer.h"

int main(int argc, char** argv) {

	vc::DataBase database{"tcp://127.0.0.1:3306", "root", "super_strong_password", "vcupload"};

	static const std::uint16_t port = 5000;

	std::cout << "Server running on port " << port << " !\n";

	vc::WebServer server {port};

	return 0;
}
