#include <iostream>

#include "DataBase.h"

int main(int argc, char** argv) {

    vc::DataBase database{"tcp://127.0.0.1:3306", "root", "super_strong_password", "vcupload"};

	
	return 0;
}
