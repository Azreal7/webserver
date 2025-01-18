#include "../include/Server.h"
#include "../include/Socket.h"
#include "../include/InetAddress.h"
#include "../include/Channel.h"
#include "../include/Acceptor.h"
#include "../include/Connection.h"
#include "../include/EventLoop.h"
#include "../include/ThreadPool.h"
#include <functional>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <thread>

#define READ_BUFFER 1024

Server::Server(EventLoop *_loop) : mainReactor(_loop), acceptor(nullptr)
{
    acceptor = new Acceptor(mainReactor);
    std::function<void(Socket *)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
    acceptor->setNewConnectionCallback(cb);

    int size = std::thread::hardware_concurrency();
    thpool = new ThreadPool(size);
    for (int i = 0; i < size; ++i)
    {
        subReactors.push_back(new EventLoop());
        std::function<void()> sub_loop = std::bind(&EventLoop::loop, subReactors[i]);
        thpool->add(sub_loop);
    }
}

Server::~Server()
{
    delete acceptor;
}

void Server::newConnection(Socket *sock)
{
    if (sock->getFd() != -1)
    {
        // 这一部分调度算法可重新设计
        int random = sock->getFd() % subReactors.size();
        Connection *conn = new Connection(subReactors[random], sock);

        std::function<void(Socket *)> cb = std::bind(&Server::deleteConnection, this, std::placeholders::_1);
        conn->setDeleteConnectionCallback(cb);
        connections[sock->getFd()] = conn;
    }
}

void Server::deleteConnection(Socket *sock)
{
    if (sock->getFd() != -1)
    {
        Connection *conn = connections[sock->getFd()];
        connections.erase(sock->getFd());
        delete conn;
    }
}