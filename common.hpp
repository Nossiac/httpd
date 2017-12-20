/**********************************************************************
 * Distributed under the terms of the GNU GPL, version 2
 * Author: Hua Shao <nossiac@163.com>
 **********************************************************************/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <map>
#include <stdexcept>


#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

#define SERVER_INFO "shttpd-v0.1"
#define AUTHOR_INFO "Hua Shao <nossiac@163.com>"


#define __DEBUG__


#define API(...)
#define FATAL(...)
#define DEBUG(...)
#define WARNING(...)
#define ERROR(...)
#define ASSERT(...)
#define TRACE(...)
#define VERBOSE(...)
#define CHKERR(...)
#define dumphex(...)


typedef std::map<std::string, std::string> http_options;
extern std::string http_time(time_t now);
extern time_t get_http_time(const char * str);

