#include <iostream>

#include "DataBase.h"

#include "WebServer.h"

int main(int argc, char** argv) {

	vc::DataBase database{"tcp://127.0.0.1:3306", "root", "super_strong_password", "vcupload"};

	vc::WebServer server {5000};

	return 0;
}
