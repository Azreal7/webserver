#include "../include/EventLoop.h"
#include "../include/Epoll.h"
#include "../include/Channel.h"
#include "../include/ThreadPool.h"
#include <functional>
#include <vector>

EventLoop::EventLoop() : ep(nullptr), quit(false), threadPool(nullptr)
{
    ep = new Epoll();
    threadPool = new ThreadPool();
}

EventLoop::~EventLoop()
{
    delete ep;
}

void EventLoop::loop()
{
    while (!quit)
    {
        std::vector<Channel *> chs;
        chs = ep->poll();
        for (auto it = chs.begin(); it != chs.end(); ++it)
        {
            (*it)->handleEvent();
        }
    }
}

void EventLoop::updateChannel(Channel *channel)
{
    ep->updateChannel(channel);
}

void EventLoop::addThread(std::function<void()> func)
{
    threadPool->add(func);
}