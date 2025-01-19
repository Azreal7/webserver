#include "../include/Channel.h"
#include <unistd.h>
#include "../include/EventLoop.h"

Channel::Channel(EventLoop *_loop, int _fd)
	: loop(_loop), fd(_fd), events(0), revents(0), inEpoll(false), useThreadPool(true) {}

Channel::~Channel() {
	if (fd != -1) {
		close(fd);
		fd = -1;
	}
}

void Channel::enableReading() {
	events = EPOLLIN | EPOLLET;
	loop->updateChannel(this);
}

int Channel::getFd() { return fd; }

uint32_t Channel::getEvents() { return events; }

uint32_t Channel::getRevents() { return revents; }

bool Channel::getInEpoll() { return inEpoll; }

void Channel::setInEpoll(bool _inEpoll) { inEpoll = _inEpoll; }

void Channel::setRevents(uint32_t _ev) { revents = _ev; }

void Channel::handleEvent() {
	if (revents & (EPOLLIN | EPOLLPRI)) {
		if (useThreadPool)
			loop->addThread(readCallback);
		else
			readCallback();
	}
	if (revents & (EPOLLOUT)) {
		if (useThreadPool)
			loop->addThread(writeCallback);
		else
			writeCallback();
	}
}

void Channel::setReadCallback(std::function<void()> _cb) { readCallback = _cb; }

void Channel::useET() {
	events |= EPOLLET;
	loop->updateChannel(this);
}

void Channel::setUseThreadPool(bool use) { useThreadPool = use; }