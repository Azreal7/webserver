#pragma once
#include <functional>
#include "Connection.h"

class Channel;
class Socket;
class EventLoop;
class Connection {
private:
    EventLoop *loop;
    Socket *sock;
    Channel *channel;
    std::function<void(Socket*)> deleteConnectionCallback;
public:
    Connection(EventLoop *_loop, Socket *_sock);
    ~Connection();

    void echo(int);
    void setDeleteConnectionCallback(std::function<void(Socket*)>);
};