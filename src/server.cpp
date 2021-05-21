#include "server.h"


Server::Server(unsigned short port) : Socket("0.0.0.0", port) { //default port = 4815U
	try {
		sock_bind();
		serv_listen();
	}
	catch (std::exception&) { throw; }

	exit_cmd = "/serv_shutdown";
	serv_works = true;
	FD_ZERO(&clients_set);

	std::cout << " > Server is ready" << std::endl;
};

void Server::serv_listen() {
	try {
		int res = listen(sock_get(), 5);
		if (res == -1)
			throw std::runtime_error("Could not start listening requests\n");
	}
	catch (std::exception&) { throw; }
	return;
}

#ifdef __linux__
void Server::serv_nodelay(int &sock, u_long argp) {
	try {
		int flags;

		if (argp != 0) {
			flags = SOCK_NONBLOCK;
		}
		else {
			flags = fcntl(sock, F_GETFL);
			if (flags == -1)
				throw std::runtime_error("Could not change socket option\n");

			flags &= ~SOCK_NONBLOCK;
		}

		int res = fcntl(sock, F_SETFL, flags);
		if (res == -1)
			throw std::runtime_error("Could not change socket option\n");
	}
	catch (std::exception&) { throw; }
	return;
}
#elif _WIN32
void Server::serv_nodelay(int &sock, u_long argp) {
	try {
		int res = ioctlsocket(sock, FIONBIO, &argp);
		if (res == SOCKET_ERROR)
			throw std::runtime_error("Could not change socket option\n");
	}
	catch (std::exception&) { throw; }
	return;
}
#endif

void Server::serv_accept() {
	int tmp = 0;

	try {
		if (sock_exists()) {
			//td::cout << " > Starting Accept" << std::endl;

			/*if (!clients.empty()) {
				Server::serv_nodelay(sock_get(), 1U);
				nodelay = true;
			}*/
			while (tmp == 0) {
				//std::cout << " > ";
				clients.push_back(tmp);
				std::vector<int>::iterator new_client = clients.end() - 1;
				*new_client = accept(sock_get(), NULL, NULL);

				if (*new_client == -1) {
#ifdef _WIN32
					if (WSAGetLastError() != WSAEWOULDBLOCK)
						throw std::runtime_error("Error while accepting connections");
					else
						++tmp;
#elif __linux__
					if (errno != EWOULDBLOCK)
						throw std::runtime_error("Error while accepting connections");
					else
						++tmp;
#endif

					clients.pop_back();
				}
			}				
		}
	}
	catch (std::exception&) { throw; }
	return;
}

void Server::serv_fdset() {
	FD_ZERO(&clients_set);

	for (size_t i = 0; i < clients.size(); ++i) {
		FD_SET(clients[i], &clients_set);
	}

	return;
}

void Server::serv_select() {
	timeval time = {0, 0};

	serv_fdset();
	select(clients.size(), &clients_set, NULL, NULL, &time);
	return;
}

void Server::serv_speak() {
	std::string message;

	for (size_t i = 0; i < clients.size(); ++i) {
		if (FD_ISSET(clients[i], &clients_set)) {
			int res = serv_recv(clients[i], message);

			if (res <= 0) {
				sock_close(clients[i]);
				std::vector<int>::iterator iter = clients.begin() + i;
				clients.erase(iter);

				std::cout << " > Client " << i << " disconnected" << std::endl;

				if (clients.empty())
					std::cout << " > All clients are disconnected" << std::endl;
			}
			else {
				std::cout << " > Message from " << i << ": \"" << message << '\"' << std::endl;
				SQL_analyser.SQL_analyse(message);
				serv_send(clients[i], SQL_analyser.SQL_get_result());
			}
		}
	}
	return;
}

void Server::serv_checkstop() {
	fd_set in;
	timeval time = { 0, 0 };
	std::string message;

	FD_ZERO(&in);
	FD_SET(0, &in);
	select(1, &in, NULL, NULL, &time);
	if (FD_ISSET(0, &in)) {
		std::cin >> message;
		if (message == exit_cmd)
			serv_works = false;
	}
	return;
}

void Server::serv_send(int &sock, const std::string &message) {
	try {
		send(sock, message.c_str(), message.length(), 0);	//Waits for delivery confirmation
	}
	catch (std::exception&) { throw; }
	return;
}

int Server::serv_recv(int &sock, std::string &message) {
	int leng = 0;

	try {
		int res = 0;
		char buf[101];

		message.erase();
		do {
			res = recv(sock, buf, 100, 0);

			if (res > 0) {
				std::string strbuf = buf;

				strbuf.resize(res);
				message += strbuf;
			}
			leng += res;
		} while (res == 100);
		//std::cout << "Result recv: " << leng << std::endl;
	}
	catch (std::exception&) { throw; }
	return leng;
}

// main server loop
void Server::loop() {
	try {
		while (serv_works) {
#ifdef __LINUX__
			sleep(1);
#elif _WIN32
			Sleep(1);
#endif
			serv_nodelay(sock_get(), 1U);
			serv_accept();
			serv_select();
			serv_nodelay(sock_get(), 1U);
			serv_speak();
			//serv_checkstop();
		}
	}
	catch (std::exception&) { throw; }
	
	stop();
	return;
}
