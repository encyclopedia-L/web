#include "Server.h"
#include "base/Logging.h"
#include "Util.h"
#include "HttpData.h"
#include <functional>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

Server::Server(EventLoop *loop_, int threadNum_, int port_)
:loop(loop_),threadNum(threadNum_),eventLoopThreadPool(new EventLoopThreadPool(loop,threadNum)),
started(false),acceptChannel(new Channel(loop)),port(port_),listenFd(socket_bind_listen(port))
{
    acceptChannel->setFd(listenFd);
    handle_for_sigpipe();
    if(setSocketNonBlocking(listenFd) < 0)
    {
        perror("set socket non block failed");
        abort();
    }
}

void Server::start()
{
    eventLoopThreadPool->start();
    acceptChannel->setEvents(EPOLLIN | EPOLLET);
    acceptChannel->setReadHandler(std::bind(&Server::handNewConn, this));
    acceptChannel->setConnHandler(std::bind(&Server::handThisConn, this));
    loop->addToPoller(acceptChannel, 0);
    started = true;
}

void Server::handNewConn()
{
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    socklen_t client_addr_len = sizeof(client_addr);
    int accept_fd = 0;
    while((accept_fd = accept(listenFd, (struct sockaddr*)&client_addr, &client_addr_len)) > 0)
    {
        EventLoop *loop_ = eventLoopThreadPool->getNextLoop();
        LOG << "New connection from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port);
        if (accept_fd >= MAXFDS)
        {
            close(accept_fd);
            continue;
        }
        if (setSocketNonBlocking(accept_fd) < 0)
        {
            LOG << "Set non block failed!";
            return;
        }
        setSocketNodelay(accept_fd);
        std::shared_ptr<HttpData> req_info(new HttpData(loop_, accept_fd));
        req_info->getChannel()->setHolder(req_info);
        loop_->queueInLoop(std::bind(&HttpData::newEvent, req_info));
    }
    acceptChannel->setEvents(EPOLLIN | EPOLLET);
}
