#pragma once
#include <functional>
#include "Connection.h"

class Channel;
class Socket;
class EventLoop;
class Buffer;
class Connection
{
private:
    EventLoop *loop;
    Socket *sock;
    Channel *channel;
    std::function<void(Socket *)> deleteConnectionCallback;
    Buffer *readBuffer;
    Buffer *writeBuffer;

public:
    Connection(EventLoop *_loop, Socket *_sock);
    ~Connection();

    // 回显，若要改为http，则需要修改
    void echo(int);
    void setDeleteConnectionCallback(std::function<void(Socket *)>);
};