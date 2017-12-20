/**********************************************************************
 * Distributed under the terms of the GNU GPL, version 2
 * Author: Hua Shao <nossiac@163.com>
 **********************************************************************/

#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include <map>
#include <list>
#include <string>

#include "request.hpp"
#include "response.hpp"
#include "connection.hpp"


class Server
{
protected:
    long served;
    virtual void handleConnection() {}
    void nonblock(int fd);

public:
    int port;
    int sock;

    Server(int port);
    virtual ~Server();

    virtual void run() {}
    Connection * getConnection(int sock);


    std::list<Connection *> connections;
};

class ServerImplSelect : public Server
{
protected:
    void handleConnection();

public:
    void run();

    ServerImplSelect(int port);
    ~ServerImplSelect();
};

#ifdef __linux__
class ServerImplEpoll : public Server
{
private:
    int epfd;

protected:
    void handleConnection();

public:
    void run();
    ServerImplEpoll(int port);
    ~ServerImplEpoll();
};
#endif







#endif /* __SERVER_HPP__ */

