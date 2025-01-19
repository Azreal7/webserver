#pragma once
#include <sys/epoll.h>
#include <functional>

class EventLoop;
class Channel {
private:
	EventLoop *loop;  // 指向分配到的epoll类
	int fd;
	uint32_t events;   // 希望监听的事件
	uint32_t revents;  // epoll返回Channel时发生的事件
	bool inEpoll;	   // 表示Channel是否已在epoll红黑树中
	bool useThreadPool;
	std::function<void()> readCallback;
	std::function<void()> writeCallback;

public:
	Channel(EventLoop *_loop, int _fd);
	~Channel();

	void handleEvent();
	void enableReading();

	int getFd();
	uint32_t getEvents();
	uint32_t getRevents();
	bool getInEpoll();
	void setInEpoll(bool);
	void useET();

	// void setEvents(uint32_t);
	void setRevents(uint32_t);
	void setReadCallback(std::function<void()>);
	void setUseThreadPool(bool use);
};