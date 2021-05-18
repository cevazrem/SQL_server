#include "main.h"


int main(int argc, char* argv[]) {
	std::string arg1, arg2;

	switch (argc) {
	case 1: {
		std::cout << " > Starting server" << std::endl;
		arg1 = "s";

		break;
	}
	case 2: {
		arg1 = argv[1];

		if (arg1 == "-s" || arg1 == "--server") {
			std::cout << " > Starting server" << std::endl;
			arg1 = "s";
		}
		else if (arg1 == "-c" || arg1 == "--client") {
			std::cout << "Error: Lacking server IPv4 address" << std::endl;
			return 0;
		}
		else {
			std::cout << "Error: Wrong parameters" << std::endl;
			return 0;
		}
		break;
	}
	case 3: {
		arg1 = argv[1];
		arg2 = argv[2];

		if (arg1 == "-s" || arg1 == "--server") {
			std::cout << " > Starting server" << std::endl;
			arg1 = "s";
		}
		else if (arg1 == "-c" || arg1 == "--client") {
			std::cout << " > Starting client" << std::endl;
			arg1 = "c";
		}
		else if (arg2 == "-c" || arg2 == "--client") {
			std::cout << " > Starting client" << std::endl;
			arg2 = arg1;
			std::cout << " > IPv4: " << arg2 << std::endl;
			arg1 = "c";
		}
		else {
			std::cout << "Error: Wrong parameters" << std::endl;
			return 0;
		}
		break;
	}
	}

	std::cout << " > Debug info:" << std::endl;
	std::cout << " > argc = " << argc << std::endl;

#ifdef _WIN32
	WSADATA ws;
	if (FAILED(WSAStartup(MAKEWORD(2, 2), &ws))) {
		int error = WSAGetLastError();
	}
#endif
	try {
		if (arg1 == "s") {
			Server serv;
			serv.loop();
			serv.stop();
		}
		else if (arg1 == "c") {
			Client clnt(arg2.c_str());
			clnt.loop();
			clnt.stop();
		}
		else {
			std::cout << "Error: unforseen error occured" << std::endl;
		}
	}
	catch (std::exception & E) { std::cout << " > ERROR: " << E.what() << std::endl; }

	return 0;
}
