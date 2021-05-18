#include "socket.h"


Socket::Socket(const char* ipv4_addr, u_short port) { //default port = 4815U
	socket_d = 0;
	sock_data.sin_family = AF_INET;

	try {
		if (port < 1025U)
			throw std::invalid_argument("Bad port number, using default port = 4815\n");
		sock_data.sin_port = htons(port);
	}
	catch (std::invalid_argument &E) {
		std::cout << " > Warning: " << E.what() << std::endl;
		sock_data.sin_port = htons(4815U);
	}

	try {
		u_long address = inet_addr(ipv4_addr);
		
		if (address == INADDR_NONE)
			throw std::invalid_argument("Bad IP-address\n");
		
		sock_data.sin_addr.s_addr = address;

		socket_d = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_d < 0) {
			throw std::runtime_error("Could not create socket descriptor\n");
		}

	}
	catch (std::exception&) { throw; }
	return;
}

int& Socket::sock_get() {
	return socket_d;
}

bool Socket::sock_exists() const {
	if (socket_d > 0)
		return true;

	return false;
}

void Socket::sock_bind() {
	try {
		if (sock_exists()) {
			int res = bind(socket_d, (sockaddr*) &sock_data, sizeof(sock_data));
			if (res < 0)
				throw std::runtime_error("Could not bind socket\n");
		}
		else
			throw std::invalid_argument("Socket is closed\n");
	}
	catch (std::exception&) { throw; }

	std::cout << " > Binding success" << std::endl;
	return;
}

void Socket::sock_connect() {
	try {
		if (sock_exists()) {
			int res = connect(socket_d, (sockaddr*) &sock_data, sizeof(sock_data));
			if (res < 0)
				throw std::runtime_error("Could not connect to server\n");
		}
		else
			throw std::invalid_argument("Socket is closed\n");
	}
	catch (std::exception&) { throw; }

	std::cout << " > Connection success" << std::endl;
	return;
}

#ifdef __linux__
void Socket::sock_close(int &sock) {
	close(sock);
	sock = 0;
	return;
}
#elif _WIN32
void Socket::sock_close(int &sock) {
	closesocket(sock);
	sock = 0;
	return;
}
#endif

void Socket::stop() {
	sock_close(socket_d);
	return;
}
