#pragma once

#include "socket.h"
#include <string>

#ifdef _WIN32
#include <Windows.h>
#elif __LINUX__
#include <unistd.h>
#endif


class Client : public Socket
{
	std::string exit_cmd;
	bool client_works;

public:
	Client(const char* ipv4_addr, u_short port = 4815U);
	void client_send(std::string& message);
	int  client_recv(std::string& message);

	void loop();
};
