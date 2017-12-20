/**********************************************************************
 * Distributed under the terms of the GNU GPL, version 2
 * Author: Hua Shao <nossiac@163.com>
 **********************************************************************/

#ifndef __REQUEST_HPP__
#define __REQUEST_HPP__

#include <map>
#include <string>

#include "common.hpp"
#include "stream.hpp"


typedef enum
{
    HTTP_INVALID = 0,

    HTTP_GET,
    HTTP_HEAD,
    HTTP_POST,

} http_method;


class Request
{
private:
    http_method method;
    std::string method_str;
    std::string version;
    std::string url;
    http_options options;

public:
    Request();
    ~Request();

    bool buildRequest(HttpStream & stream);
    bool buildRequest(const unsigned char * const s, int len);

    http_method getMethod();
    std::string getOption(const char * key);
    std::string getVersion();
    std::string getURI();
    std::string getHost();
    void dump() {}
};

#endif /* __REQUEST_HPP__ */

