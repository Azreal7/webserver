#pragma once
#include <functional>
#include "Connection.h"

class Channel;
class Socket;
class EventLoop;
class Buffer;
class Connection {
public:
	enum State { Invalid, Handshaking, Connected, Closed, Failed };
	Connection(EventLoop *_loop, Socket *_sock);
	~Connection();

	void Read();
	void Write();
	void close();
	State getState();
	void setSendBuffer(const char *str);
	Buffer *getReadBuffer();
	const char *getCharReadBuffer();
	Buffer *getWriteBuffer();
	const char *getCharWriteBuffer();
	void getlineSendBuffer();
	Socket *getSocket();
	void onConnect(std::function<void()> fn);
	void setOnConnectCallback(std::function<void(Connection *)> const &callback);
	void setDeleteConnectionCallback(std::function<void(Socket *)> const &callback);

private:
	EventLoop *loop;
	Socket *sock;
	Channel *channel{nullptr};
	State state{State::Invalid};
	std::function<void(Socket *)> deleteConnectionCallback;
	Buffer *readBuffer{nullptr};
	Buffer *writeBuffer{nullptr};

	std::function<void(Connection *)> onConnectCallback;

	void readNonBlocking();
	void writeNonBlocking();
	void readBlocking();
	void writeBlocking();
};
