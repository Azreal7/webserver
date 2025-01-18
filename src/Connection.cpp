#define READ_BUFFER 1024
#include "../include/Connection.h"
#include "../include/Socket.h"
#include "../include/Channel.h"
#include "../include/Buffer.h"
#include "../include/Server.h"
#include "../include/util.h"
#include <cstdio>
#include <cerrno>
#include <unistd.h>
#include <cstring>

Connection::Connection(EventLoop *_loop, Socket *_sock) : loop(_loop), sock(_sock), channel(nullptr), readBuffer(nullptr), writeBuffer(nullptr)
{
    readBuffer = new Buffer();
    writeBuffer = new Buffer();
    channel = new Channel(loop, sock->getFd());
    std::function<void()> cb = std::bind(&Connection::echo, this, sock->getFd());
    channel->setReadCallback(cb);
    channel->setUseThreadPool(true);
    channel->enableReading();
}

Connection::~Connection()
{
    delete channel;
    delete sock;
}

void Connection::echo(int sockfd)
{
    // printf("Connection working!\n");
    // 回显sockfd发来的数据
    char buf[READ_BUFFER];
    while (true)
    { // 由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if (bytes_read > 0)
        {
            readBuffer->append(buf, bytes_read);
        }
        else if (bytes_read == -1 && errno == EINTR)
        { // 客户端正常中断、继续读取
            printf("continue reading");
            continue;
        }
        else if (bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        { // 非阻塞IO，这个条件表示数据全部读取完毕
            printf("message from client fd %d: %s\n", sockfd, readBuffer->c_str());

            // 写缓冲区
            writeBuffer->append(readBuffer->c_str(), readBuffer->size());
            errif(write(sockfd, writeBuffer->c_str(), writeBuffer->size()) == -1, "socket write error");

            readBuffer->clear();
            writeBuffer->clear();
            break;
        }
        else if (bytes_read == 0)
        { // EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            deleteConnectionCallback(sock);
            // printf("1\n");
            break; // 删除完还能break吗？居然还真能，为什么呢？
        }
    }
}

void Connection::setDeleteConnectionCallback(std::function<void(Socket *)> _cb)
{
    deleteConnectionCallback = _cb;
}