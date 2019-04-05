/**********************************************************************
 * Distributed under the terms of the GNU GPL, version 2
 * Author: Hua Shao <nossiac@163.com>
 **********************************************************************/

#ifndef __RESPONSE_HPP__
#define __RESPONSE_HPP__

#include <map>
#include <vector>
#include <string>

#include "common.hpp"
#include "buffer.hpp"
#include "stream.hpp"


typedef struct
{
    int i_code;
    const char * s_code;
    const char * s_expl;
} http_status;


class Response
{
protected:
    int code;
    static http_status * get_http_status(int code);

public:

    //std::vector<unsigned char> data;
    std::string version;
    std::string code_str;
    std::string code_expl;
    http_options options;
    Response();
    ~Response();

    std::string getOption(const char * key);
    void setOption(const char * key, size_t value);
    void setOption(const char * key, const char * value);
    void setCode(int code);
    void setVersion(std::string httpver);

#ifdef __DEBUG__
    void dump();
#endif
};

#endif /* __RESPONSE_HPP__ */

