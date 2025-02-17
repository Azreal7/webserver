#include "../include/Epoll.h"
#include <string.h>
#include <unistd.h>
#include "../include/Channel.h"
#include "../include/util.h"

#define MAX_EVENTS 1000

Epoll::Epoll() : epfd(-1), events(nullptr) {
	epfd = epoll_create1(0);
	errif(epfd == -1, "epoll create error");
	events = new epoll_event[MAX_EVENTS];
	bzero(events, sizeof(*events) * MAX_EVENTS);
}

Epoll::~Epoll() {
	if (epfd != -1) {
		close(epfd);
		epfd = -1;
	}
	delete[] events;
}

void Epoll::addFd(int fd, uint32_t op) {
	struct epoll_event ev;
	bzero(&ev, sizeof(ev));
	ev.data.fd = fd;
	ev.events = op;
	errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add event error");
}

std::vector<Channel *> Epoll::poll(int timeout) {
	std::vector<Channel *> activeEvents;
	int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
	errif(nfds == -1, "epoll wait error");
	for (int i = 0; i < nfds; ++i) {
		Channel *ch = (Channel *)events[i].data.ptr;
		ch->setRevents(events[i].events);
		activeEvents.push_back(ch);
	}
	return activeEvents;
}

void Epoll::updateChannel(Channel *channel) {
	int fd = channel->getFd();
	struct epoll_event ev;
	bzero(&ev, sizeof(ev));
	ev.data.ptr = channel;
	ev.events = channel->getEvents();  // 能否监听、边缘或水平触发
	if (!channel->getInEpoll()) {
		errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add error");
		channel->setInEpoll(true);
	} else {
		errif(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll modify error");	 // 已存在，则修改
	}
}

void Epoll::deleteChannel(Channel *ch) {
	int fd = ch->getFd();
	errif(epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr) == -1, "epoll delete error");
	ch->setInEpoll(false);
}