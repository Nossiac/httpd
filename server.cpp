/**********************************************************************
 * Distributed under the terms of the GNU GPL, version 2
 * Author: Hua Shao <nossiac@163.com>
 **********************************************************************/

#include <iostream>

#include <errno.h>
#include <string.h>

#include <unistd.h>
#include <getopt.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdexcept>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>


#ifdef __linux__
#include <sys/epoll.h>
#endif

#define STDIN (0)
#include "common.hpp"
#include "request.hpp"
#include "response.hpp"
#include "server.hpp"

using namespace std;



Server::Server(int port)
{
    struct sockaddr_in servaddr;
    const int opt = 1;

    this->served = 0;
    this->port = port;

    this->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (this->sock < 0)
    {
        throw std::runtime_error(std::string("socket creation error : ")+std::string(strerror(errno)));
    }

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
    }

    if (setsockopt(this->sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0)
    {
    }

#if 0
    https://stackoverflow.com/questions/3757289/tcp-option-so-linger-zero-when-its-required
    struct linger so_linger;
    so_linger.l_onoff = TRUE;
    so_linger.l_linger = 0;
    if (setsockopt(this->sock, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger))<0)
    {
    }
#endif

    if (bind(this->sock, (struct sockaddr*)&servaddr, sizeof(struct sockaddr)) != 0)
    {
        throw std::runtime_error(std::string("socket binding error : ")+std::string(strerror(errno)));
    }
}


Server::~Server()
{
    close(this->sock);
}


Connection * Server::getConnection(int sock)
{
    list<Connection *>::iterator iter;
    for (iter=this->connections.begin(); iter!=this->connections.end(); ++iter)
    {
        if (sock == (*iter)->sock)
            return (*iter);
    }

    return NULL;
}

/*
    ET mode epoll should use non-blocking socket.
    http://man7.org/linux/man-pages/man7/epoll.7.html
*/
void Server::nonblock(int fd)
{
    int opts;
    opts = fcntl(fd, F_GETFL);
    if(opts < 0)
    {
        return;
    }

    opts = opts | O_NONBLOCK;
    if(fcntl(fd, F_SETFL, opts) < 0)
    {
        return;
    }
}



#ifdef __linux__
ServerImplEpoll::ServerImplEpoll(int port) : Server(port)
{
    struct epoll_event ev;


    /* ET mode, non-blocking. */
    nonblock(this->sock);

    this->epfd = epoll_create1(0);
    ev.data.fd = this->sock;
    ev.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(this->epfd, EPOLL_CTL_ADD, this->sock, &ev) < 0)
    {
        throw std::runtime_error(std::string("epoll_ctl error : ")+std::string(strerror(errno)));
    }
}


ServerImplEpoll::~ServerImplEpoll()
{
    close(this->epfd);
}


void ServerImplEpoll::handleConnection()
{
    struct epoll_event ev;
    /* ET mode, need drain the event && data. */
    while (true)
    {
        int sock = accept(this->sock, NULL, NULL);
        if (sock < 0)
            break;
        /* special cases of accept failure:
        1) EAGAIN or EWOULDBLOCK, no connection under non-blocking mode
        2) EINTR, interrupted before connection arrives under blocking mode
        */

        nonblock(sock);

        ev.data.fd = sock;
        ev.events = EPOLLIN | EPOLLET;
        if (epoll_ctl(this->epfd, EPOLL_CTL_ADD, sock, &ev) < 0)
        {
            return;
        }

        served++;
        connections.push_back(new Connection(sock));
    }
}



void ServerImplEpoll::run()
{
    #define MAXEVENT (32)
    int nfds, i;
    struct epoll_event events[MAXEVENT];
    list<Connection *>::iterator iter;
    int ms = 10000;
    Connection * conn = NULL;

    if(listen(this->sock, 16) != 0)
    {
        return;
    }

    while (true)
    {
        nfds = epoll_wait(this->epfd, events, MAXEVENT, ms);
        if (nfds < 0)
        {
            return;
        }
        for (i = 0; i < nfds; ++i)
        {

            if (events[i].data.fd == this->sock)
            {
                this->handleConnection();
            }
            else if(events[i].events & EPOLLIN)
            {
                conn = getConnection(events[i].data.fd);
                if (!conn) continue;

                conn->handleStream();

                if (conn->peerclosed() || conn->shouldclose())
                {
                    epoll_ctl(this->epfd, EPOLL_CTL_DEL, conn->sock, 0);
                    this->connections.remove(conn);
                    delete conn;
                    continue;
                }
            }
        }
    }

}

#endif /* __linux__ */


ServerImplSelect::ServerImplSelect(int port) : Server(port)
{
    nonblock(sock);
}


ServerImplSelect::~ServerImplSelect()
{
}


void ServerImplSelect::handleConnection()
{
__again:
    int sock = accept(this->sock, NULL, NULL);
    if (sock < 0)
    {
        if (errno == EINTR) goto __again;
        return;
    }

    nonblock(sock);
    this->served++;
    this->connections.push_back(new Connection(sock));
}


void ServerImplSelect::run()
{
    Connection * conn = NULL;
    list<Connection *>::iterator iter;

    if(listen(this->sock, 16) != 0)
    {
        return;
    }


    while (true)
    {
        list<Connection *>::iterator iter;
        fd_set readfds;
        int maxfd = this->sock;
        int ret;

        struct timeval tv;
        tv.tv_sec = 10;
        tv.tv_usec = 0;

        FD_ZERO(&readfds);
        FD_SET(this->sock, &readfds);
        for (iter=this->connections.begin(); iter!=this->connections.end(); ++iter)
        {
            if ((*iter)->sock > maxfd) maxfd = (*iter)->sock;
            FD_SET((*iter)->sock, &readfds);
        }

        ret = select(maxfd+1, &readfds, NULL, NULL, &tv);
        if (ret < 0)
        {
            continue;
        }

        if (FD_ISSET(this->sock, &readfds))
            this->handleConnection();

        for (iter=this->connections.begin(); iter!=this->connections.end(); ++iter)
        {
            conn = (*iter);
            if (FD_ISSET(conn->sock, &readfds))
            {
                conn->handleStream();
                if (conn->peerclosed() || conn->shouldclose())
                {
                    this->connections.erase(iter++);
                    delete conn;
                    continue;
                }
            }
        }

    }
}

