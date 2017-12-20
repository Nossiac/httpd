/**********************************************************************
 * Distributed under the terms of the GNU GPL, version 2
 * Author: Hua Shao <nossiac@163.com>
 **********************************************************************/

#include <iostream>
#include "stream.hpp"
#include "request.hpp"

using namespace std;

Request::Request()
{


}

bool Request::buildRequest(const unsigned char * const s, int len)
{
    HttpStream stream(s, len);
    return this->buildRequest(stream);
}

bool Request::buildRequest(HttpStream & stream)
{
    this->method_str = stream.readword();
    if (this->method_str.size() == 0) return false;

    if (this->method_str == "GET")
        this->method = HTTP_GET;
    else if (this->method_str == "HEAD")
        this->method = HTTP_HEAD;
    else if (this->method_str == "POST")
        this->method = HTTP_POST;
    else
    {
        return false;
    }

    stream.skipblank();
    this->url = stream.readword();
    if (0 == this->url.size()) return false;
    stream.skipblank();
    this->version = stream.readword();
    if (0 == this->version.size()) return false;
    stream.skipblank();

    while(true)
    {
        string line = stream.readline();
        if (0 == line.size())
        {
            break;
        }
        size_t f = line.find_first_of(':');
        if (f == string::npos)
        {
            return false;
        }
        string key = line.substr(0, f);
        while (line[++f] == ' ');
        string val = line.substr(f);
        this->options.insert(pair<string, string>(key, val));
    }

    return true;
}


Request::~Request()
{
}


string Request::getOption(const char * key)
{
    if (this->options.count(key))
        return this->options[key];
    return string();
}


http_method Request::getMethod()
{
    return this->method;
}


string Request::getVersion()
{
    return this->version;
}


string Request::getURI()
{
    return this->url;
}


string Request::getHost()
{
    return "";
}



