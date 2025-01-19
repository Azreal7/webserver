#define READ_BUFFER 1024
#include "../include/Connection.h"
#include <sys/socket.h>
#include <unistd.h>
#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <utility>
#include "../include/Buffer.h"
#include "../include/Channel.h"
#include "../include/Server.h"
#include "../include/Socket.h"
#include "../include/util.h"

Connection::Connection(EventLoop *_loop, Socket *_sock) : loop(_loop), sock(_sock) {
	if (loop != nullptr) {
		channel = new Channel(loop, sock->getFd());
		channel->setUseThreadPool(true);
		channel->enableReading();
		channel->useET();
	}

	readBuffer = new Buffer();
	writeBuffer = new Buffer();
}

Connection::~Connection() {
	if (loop != nullptr) {
		delete channel;
	}
	delete readBuffer;
	delete writeBuffer;
	delete sock;
}

void Connection::Read() {
	readBuffer->clear();
	if (sock->isNonBlocking()) {
		readNonBlocking();
	} else {
		readBlocking();
	}
}

void Connection::Write() {
	if (sock->isNonBlocking()) {
		writeNonBlocking();
	} else {
		writeBlocking();
	}
	writeBuffer->clear();
}

void Connection::close() { deleteConnectionCallback(sock); }

Connection::State Connection::getState() { return state; }

void Connection::setSendBuffer(const char *str) { writeBuffer->setBuf(str); }

Buffer *Connection::getReadBuffer() { return readBuffer; }

const char *Connection::getCharReadBuffer() { return readBuffer->c_str(); }

Buffer *Connection::getWriteBuffer() { return writeBuffer; }

const char *Connection::getCharWriteBuffer() { return writeBuffer->c_str(); }

void Connection::getlineSendBuffer() { writeBuffer->getline(); }

Socket *Connection::getSocket() { return sock; }

void Connection::setOnConnectCallback(std::function<void(Connection *)> const &callback) {
	onConnectCallback = callback;
	channel->setReadCallback([this]() { this->onConnectCallback(this); });
}

void Connection::setDeleteConnectionCallback(std::function<void(Socket *)> const &callback) {
	deleteConnectionCallback = callback;
}

void Connection::onConnect(std::function<void()> fn) {}

void Connection::readNonBlocking() {
	int sockfd = sock->getFd();
	char buf[1024];
	while (true) {
		memset(buf, 0, sizeof(buf));
		ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
		if (bytes_read > 0) {
			readBuffer->append(buf, bytes_read);
		} else if (bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
			break;
		} else if (bytes_read == 0) {
			printf("read EOF, client fd %d disconnected\n", sockfd);
			state = State::Closed;
			break;
		} else {
			printf("Other error on client fd %d\n", sockfd);
			state = State::Closed;
			break;
		}
	}
}

void Connection::writeNonBlocking() {
	int sockfd = sock->getFd();
	int dataSize = writeBuffer->size();
	char buf[dataSize];
	memcpy(buf, writeBuffer->c_str(), dataSize);
	int dataLeft = dataSize;

	while (dataLeft > 0) {
		ssize_t byte_write = write(sockfd, buf + dataSize - dataLeft, dataLeft);
		if (byte_write == -1 && errno == EINTR) {
			printf("continue writing\n");
			continue;
		}
		if (byte_write == -1 && errno == EAGAIN) {
			break;
		}
		if (byte_write == -1) {
			printf("Other error on client fd %d\n", sockfd);
			state = State::Closed;
			break;
		}
		dataLeft -= byte_write;
	}
}

void Connection::readBlocking() {
	int sockfd = sock->getFd();
	unsigned int rcv_size = 0;
	socklen_t len = sizeof(rcv_size);
	getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcv_size, &len);
	char buf[rcv_size];
	ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
	if (bytes_read > 0) {
		readBuffer->append(buf, bytes_read);
	} else if (bytes_read == 0) {
		printf("read EOF, blocking client fd %d disconnected\n", sockfd);
		state = State::Closed;
	} else if (bytes_read == -1) {
		printf("Other error onblocking client fd %d\n", sockfd);
		state = State::Closed;
	}
}

void Connection::writeBlocking() {
	int sockfd = sock->getFd();
	ssize_t byte_write = write(sockfd, writeBuffer->c_str(), writeBuffer->size());
	if (byte_write == -1) {
		printf("Other error on blocking client fd %d\n", sockfd);
		state = State::Closed;
	}
}
