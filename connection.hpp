/**********************************************************************
 * Distributed under the terms of the GNU GPL, version 2
 * Author: Hua Shao <nossiac@163.com>
 **********************************************************************/

#ifndef __CONNECTION_HPP__
#define __CONNECTION_HPP__

#include <map>
#include <vector>
#include <string>


#include "stream.hpp"
#include "request.hpp"
#include "response.hpp"

class Connection
{
private:
    bool __peerclosed;
    bool __shouldclose;
    bool __writeagain;

    buffer rxbuf;
    buffer txbuf;

    bool handleHttpGet(Request & req, Response & rsp);
    bool handleHttpPost(Request & req, Response & rsp);
    bool handleRequest(Request & req, Response & rsp);

public:
    size_t bytes_sent;
    size_t bytes_recv;

    const int sock;
    void handleStream();
    inline void peerclosed(bool closed)
    {
        this->__peerclosed = closed;
    };
    inline bool peerclosed()
    {
        return this->__peerclosed;
    };
    inline bool shouldclose()
    {
        return this->__shouldclose;
    };
    inline void shouldclose(bool closed)
    {
        this->__shouldclose = closed;
    };
    size_t _write(int sock, void * pvdata, int len, int flags);
    size_t write(void * pvdata, int len);
    size_t write(Response &rsp);
    size_t read();
    bool blocking();

    Connection(int sock);
    ~Connection();
};


#endif /* __CONNECTION_HPP__ */
