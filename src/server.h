#pragma once

#include <vector>
#include <cerrno>
#include "socket.h"
#include "../sql/sql.h"

#ifdef _WIN32
#include <Windows.h>
#elif __LINUX__
#include <unistd.h>
#endif

class Server : public Socket
{
	std::vector <int> clients;
	std::string exit_cmd;
	fd_set clients_set;
	SQL SQL_analyser;
	bool serv_works;
	

public:
	Server(unsigned short port = 4815U);
	void serv_fdset();
	void serv_listen();
	void serv_accept();
	void serv_select();
	void serv_speak();
	void serv_checkstop();
	void serv_nodelay(int &sock, u_long argp);
	void serv_send(int &sock, const std::string &message);
	int  serv_recv(int &sock, std::string &message);

	void loop();
};
