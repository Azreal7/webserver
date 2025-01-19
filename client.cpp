#include <iostream>
#include "include/Connection.h"
#include "include/EventLoop.h"
#include "include/Server.h"
#include "include/Socket.h"

int main() {
	Socket *sock = new Socket();
	sock->connect("127.0.0.1", 1234);
	Connection *conn = new Connection(nullptr, sock);

	while (true) {
		conn->getlineSendBuffer();
		conn->Write();
		if (conn->getState() == Connection::State::Closed) {
			std::cout << "close!" << std::endl;
			conn->close();
			break;
		}
		conn->Read();
		std::cout << "Message from server: " << conn->getCharReadBuffer() << std::endl;
	}

	delete conn;
	return 0;
}
