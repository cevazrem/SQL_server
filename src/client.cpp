#include "client.h"


Client::Client(const char* ipv4_addr, u_short port) : Socket(ipv4_addr, port) { //default port = 4815U
	try {
		sock_connect();
	}
	catch (std::exception&) { throw; }

	exit_cmd = "/client_shutdown";
	client_works = true;
}

void Client::client_send(std::string& message) {
	try {
		send(sock_get(), message.c_str(), message.length(), 0);	//Waits for delivery confirmation
	}
	catch (std::exception&) { throw; }
	return;
}

int Client::client_recv(std::string& message) {
	int leng = 0;

	try {
		int res = 0;
		char buf[101];

		message.erase();
		do {
			res = recv(sock_get(), buf, 100, 0);

			if (res > 0) {
				std::string strbuf = buf;

				strbuf.resize(res);
				message += strbuf;
			}
			leng += res;
		} while (res == 100);
		//std::cout << " > Result recv: " << leng << std::endl;
		//std::cout << " > Message: " << message  << std::endl;
	}
	catch (std::exception&) { throw; }
	return leng;
}

void Client::loop() {
	std::string message = "";
	std::string reader = "";

	try {
		while (Client::client_works) {
			sleep(1);
			message.erase();
			std::cout << " > ";
			getline(std::cin, message);

			if (message == Client::exit_cmd) {
				client_works = false;
				continue;
			}

			client_send(message);
			int res = client_recv(message);

			if (res < 0)
				throw std::runtime_error("Client is disconnected from server\n");
			else {
				std::cout << message << std::endl;
			}
		}
		stop();
	}
	catch (std::exception&) { throw; }

	return;
}
