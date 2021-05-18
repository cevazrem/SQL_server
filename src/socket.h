#pragma once

#include <iostream>
#include <sys/types.h>

#ifdef __linux__
	#include <netinet/in.h> 
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <unistd.h>
	#include <fcntl.h>
#elif _WIN32
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#include <winsock2.h>
	#include <WS2tcpip.h>
	#pragma comment (lib, "Ws2_32.lib")
#endif


class Socket {
	int socket_d;
	struct sockaddr_in sock_data;

public:
	Socket(const char* ipv4_addr, u_short port = 4815U);

	bool sock_exists() const;
	void sock_bind();
	void sock_connect();
	void sock_close(int &sock);
	void stop();
protected:
	int& sock_get();	//for use only in Server|Client classes
};
