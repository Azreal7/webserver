#pragma once

#include <functional>
#include <map>
#include <vector>

class EventLoop;
class Acceptor;
class Socket;
class Connection;
class ThreadPool;
class Server {
private:
	EventLoop *mainReactor;
	Acceptor *acceptor;
	std::map<int, Connection *> connections;
	std::vector<EventLoop *> subReactors;
	ThreadPool *thpool;
	std::function<void(Connection *)> onConnectCallback;

public:
	Server(EventLoop *);
	~Server();
	void newConnection(Socket *serv_sock);
	void deleteConnection(Socket *sock);
	void onConnect(std::function<void(Connection *)> fn);
};