#include "include/Server.h"
#include <iostream>
#include "include/Connection.h"
#include "include/EventLoop.h"
#include "include/Socket.h"

int main() {
	EventLoop *loop = new EventLoop();
	Server *server = new Server(loop);
	server->onConnect([](Connection *conn) {
		conn->Read();
		if (conn->getState() == Connection::State::Closed) {
			conn->close();
			return;
		}
		std::cout << "Message from client " << conn->getSocket()->getFd() << ": " << conn->getCharReadBuffer()
				  << std::endl;
		conn->setSendBuffer(conn->getCharReadBuffer());
		conn->Write();
	});
	loop->loop();
	delete loop;
	delete server;
	return 0;
}