/**********************************************************************
 * Distributed under the terms of the GNU GPL, version 2
 * Author: Hua Shao <nossiac@163.com>
 **********************************************************************/

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include "common.hpp"
#include "stream.hpp"
#include "response.hpp"
#include "connection.hpp"

using namespace std;

#define RXBUF_LEN (32)
#define MAX_SEND_RETRY (100)


Connection::Connection(int sock) : sock(sock)
{
    this->__peerclosed = false;
    this->rxbuf.capacity(RXBUF_LEN);
    this->bytes_sent = 0;
    this->bytes_recv = 0;
}


Connection::~Connection()
{
    close(this->sock);
}

bool Connection::handleHttpPost(Request & req, Response & rsp)
{
    return true;
}


bool Connection::handleHttpGet(Request & req, Response & rsp)
{
    int ret = -1;
    string filepath = req.getURI();
    struct stat filestat;
    FILE * fp = NULL;
    char buf[1024];
    size_t from = 0;
    size_t to = 0;
    size_t total = 0;
    size_t done = 0;
    string range = req.getOption("Range");

    if (filepath[0] == '/')
        filepath = "."+req.getURI();

    /* if the uri is a regular file */
    DEBUG("client request file : %s", filepath.c_str());
    ret = stat(filepath.c_str(), &filestat);
    if (ret != 0)
    {
        rsp.setCode(404);
        this->write(rsp);
        goto __quit_true;
    }
    if (!S_ISREG(filestat.st_mode))
    {
        rsp.setCode(403);
        this->write(rsp);
        goto __quit_true;
    }

    {
        /* RFC2616, 14.29. Last-Modified */
        string datestr = http_time(filestat.st_mtime);
        rsp.setOption("Last-Modified", datestr.c_str());

        /* RFC2616, 14.18. Date */
        datestr = http_time(time(0));
        rsp.setOption("Date", datestr.c_str());
    }

    /* RFC2616, 14.25. If-Modified-Since */
    {
        char * timebuf = ctime(&filestat.st_mtime);
        string time = req.getOption("If-Modified-Since");
        time_t reqtime, acctime;
        reqtime = get_http_time(time.c_str());
        acctime = get_http_time(timebuf);

        if(reqtime >= acctime)
        {
            rsp.setCode(304);
            this->write(rsp);
            goto __quit_true;
        }
    }

    /* Range */
    if (range.empty())
    {
        total = filestat.st_size;
        from = 0;
        to = total - 1;
    }
    else
    {
        std::ostringstream ss;
        const char * p = range.c_str();
        /* figure out FROM-TO */
        do
        {
            // bytes=-200
            ret = sscanf(p,"bytes=-%zu", &to);
            if (ret == 1)
            {
                from = 0;
                break;
            }
            // bytes=100-200
            ret = sscanf(p,"bytes=%zu-%zu", &from, &to);
            if (ret == 2)
            {
                break;
            }
            // bytes=200-
            ret = sscanf(p,"bytes=%zu-", &from);
            if (ret == 1)
            {
                to = filestat.st_size-1;
                break;
            }
        }
        while (0);



        total = to - from + 1;
        ss<<"bytes "<<from<<'-'<<to<<'/'<<filestat.st_size;

        DEBUG("client req from %zu to %zu, total %zu bytes", from, to, total);
        rsp.setCode(206);
        rsp.setOption("Content-Range", ss.str().c_str());
    }

    rsp.setOption("Content-Length", total);
    rsp.setOption("Accept-Ranges", "bytes");

    fp = fopen(filepath.c_str(), "rb");
    if (!fp)
    {
        rsp.setCode(404);
        this->write(rsp);
        goto __quit_true;
    }


    /* write response header! */
    ret = this->write(rsp);
    if (ret <= 0)
        goto __quit_false;

    if (req.getMethod()== HTTP_HEAD)
        goto __quit_true;


    /* write file data! */
    fseek(fp, from, SEEK_SET);
    done = 0;
    while(!feof(fp) && done < total)
    {
        size_t read = 0;
        size_t sent = 0;
        if (total-done < sizeof(buf))
            read = fread(buf, 1, total-done, fp);
        else
            read = fread(buf, 1, sizeof(buf), fp);
        ASSERT(read >= 0);
        //DEBUG("done %zu, total %zu", done, total);

        sent = this->write(buf, read);
        if (sent != read)
            goto __quit_false;
        done += sent;
    }
    DEBUG("done %zu, total %zu", done, total);
    ASSERT(done == total);

__quit_true:
    if (fp) fclose(fp);
    return true;

__quit_false:
    DEBUG("Error happend: %s, done %zu, total %zu", strerror(errno), done, total);
    if (fp) fclose(fp);
    return false;
}


bool Connection::handleRequest(Request & req, Response & rsp)
{

    rsp.setVersion(req.getVersion());
    if (req.getOption("Connection") != "Keep-Alive" || req.getVersion() == "HTTP/1.0")
    {
        rsp.setOption("Connection", "close");
        this->shouldclose(true);
    }

    switch(req.getMethod())
    {
        case HTTP_HEAD:
        /* do not break here */
        case HTTP_GET:
            rsp.setCode(200);
            return this->handleHttpGet(req, rsp);
        case HTTP_POST:
            break;
        default:
            rsp.setCode(405);
            return this->write(rsp);
    }

    return true;
}


void Connection::handleStream()
{
    Request req;
    Response rsp;

    if (this->read() <= 0)
        return;

    /* if current data is correct but insufficient to build a request,
    we keep the data in buffer, return.

        1. good request: handle request
            1) no more data;
            2) extra data, keep in buffer.
        2. invalid request: discard and feedback error
        3. partial request: wait for more
    */


    if (req.buildRequest(this->rxbuf.data(), this->rxbuf.size()))
    {
        this->handleRequest(req, rsp);
    }
    else
    {
        rsp.setCode(400);
        this->write(rsp);
    }
}


bool Connection::blocking()
{
    int opts;
    opts = fcntl(this->sock, F_GETFL);
    if(opts < 0)
    {
        return false;
    }

    return (opts & O_NONBLOCK) ? false : true;
}

size_t Connection::read()
{
    ssize_t ret = 0;
    void * buffer = NULL;
    size_t buflen = 0;
    size_t total = 0;


    do
    {
        buffer = this->rxbuf.data() + this->rxbuf.size();
        buflen = this->rxbuf.capacity() - this->rxbuf.size();

        ret = recv(this->sock, buffer, buflen, 0);
        if (0 == ret)
        {
            DEBUG("socket closed by peer!");
            this->peerclosed(true);
            break;
        }
        else if (ret < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                break; /* data drained */
            }
            else if (errno == EINTR)
            {
                continue;
            }
            else
            {
                break;
            }
        }
        else
        {
            total += ret;
            this->rxbuf.size(this->rxbuf.size() + ret);
            if (this->rxbuf.size()*2 > this->rxbuf.capacity())
                this->rxbuf.capacity(this->rxbuf.capacity()*2);
        }
    }
    while (!this->blocking());

    this->bytes_recv += total;
    return total;
}


size_t Connection::_write(int sock, void * pvdata, int len, int flags)
{
    ssize_t ret = send(this->sock, pvdata, len, 0);
    if (ret>0) this->bytes_sent += (size_t)ret;
    return ret;
}

size_t Connection::write(void * pvdata, int len)
{
    size_t todo = len;
    size_t done = 0;
    ssize_t ret = -1;
    unsigned char * p = reinterpret_cast<unsigned char *>(pvdata);
    int retry = 0;

    /* send will return:
          1) len, if everything is fine.
          2) -1,  if something is wrong (including EINTR), check errno.
        In theory, it also can return:
          3) 0
          4) between 0 and len, if syscall get interrupted halfway.
            (EINTR means no data was sent yet).
        but no system implements it like that.
    */

__again:
    ret = this->_write(this->sock, p, todo, 0);
    if (ret < 0)
    {
        if(errno == EINTR)
        {
            goto __again;
        }
        else if (errno == ECONNRESET || errno == EPIPE)
        {
            this->peerclosed(true);
            this->shouldclose(true);
            return ret;
        }
        else if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            /* A more elegant solution would be using epoll to monitor EPOLLOUT, but NO, I just keep it simple! */
            struct timespec inteval;
            inteval.tv_sec = 0;
            inteval.tv_nsec = 100000;
            nanosleep(&inteval, NULL);

            if (retry++ < MAX_SEND_RETRY)
                goto __again;
            else
                return done>0?done:ret;
        }
        else
        {
            ERROR("error: %s", strerror(errno));
            return ret;
        }
    }
    else if ((size_t)ret != todo)
    {
        p += ret;
        done += ret;
        todo -= ret;
        retry ++;
        goto __again;
    }
    else
    {
        done += ret;
    }

    ASSERT(done == (size_t)len);
    //dumphex(NULL, p, len);

    return done;
}

size_t Connection::write(Response &rsp)
{
    this->txbuf.clear();
    this->txbuf.append(rsp.version.c_str(), rsp.version.size());
    this->txbuf.append(" ", 1);
    this->txbuf.append(rsp.code_str.c_str(), 3);
    this->txbuf.append(" ", 1);
    this->txbuf.append(rsp.code_expl.c_str(), rsp.code_expl.size());
    this->txbuf.append("\r\n", 2);

    http_options::iterator iter;

    string len = rsp.getOption("Content-Length");
    if (len.empty())
        rsp.setOption("Content-Length", static_cast<size_t>(0));


    for (iter = rsp.options.begin(); iter != rsp.options.end(); ++iter)
    {
        this->txbuf.append((*iter).first.c_str(), (*iter).first.size());
        this->txbuf.append(": ", 2);
        this->txbuf.append((*iter).second.c_str(), (*iter).second.size());
        this->txbuf.append("\r\n", 2);
    }
    this->txbuf.append("\r\n", 2);

    return this->write(this->txbuf.data(), this->txbuf.size());
}

