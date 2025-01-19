#pragma once

#include <cstdint>

class InetAddress;
class Socket {
private:
	int fd;

public:
	Socket();
	explicit Socket(int);
	~Socket();

	void bind(InetAddress *);
	void listen();
	void setnonblocking();

	int accept(InetAddress *);

	int getFd();
	void connect(InetAddress *_addr);
	void connect(const char *ip, uint16_t port);

	bool isNonBlocking();
};
