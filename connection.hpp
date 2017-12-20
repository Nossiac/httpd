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
    buffer pending;

    bool handleHttpGet(Request & req, Response & rsp);
    bool handleHttpPost(Request & req, Response & rsp);
    bool handleRequest(Request & req, Response & rsp);

public:
    const int sock;
    void handleStream();
    bool handlePending();
    inline void peerclosed(bool closed)
    {
        this->__peerclosed = closed;
    };
    inline bool peerclosed()
    {
        return this->__peerclosed;
    };
    inline void writeagain(bool closed)
    {
        this->__writeagain = closed;
    };
    inline bool writeagain()
    {
        return this->__writeagain;
    };
    inline bool shouldclose()
    {
        return this->__shouldclose;
    };
    inline void shouldclose(bool closed)
    {
        this->__shouldclose = closed;
    };
    bool write(void * pvdata, int len);
    bool write(Response &rsp);
    size_t read();
    bool blocking();

    Connection(int sock);
    ~Connection();
};


#endif /* __CONNECTION_HPP__ */
