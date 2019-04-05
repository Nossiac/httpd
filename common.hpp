/**********************************************************************
 * Distributed under the terms of the GNU GPL, version 2
 * Author: Hua Shao <nossiac@163.com>
 **********************************************************************/

#ifndef __COMMON_HPP__
#define __COMMON_HPP__

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



enum
{
    DBG_LVL_NONE = 0,
    DBG_LVL_ERROR,
    DBG_LVL_WARNING,
    DBG_LVL_DEBUG,
    DBG_LVL_VERBOSE,
};


#ifdef __DEBUG__
extern int __dbg_lvl__;
#define ASSERT(stmt) \
    if (__dbg_lvl__ > DBG_LVL_NONE) if (!(stmt)) do { \
        fprintf(stderr, "<error> assertion failure: %s, @%s, #%d\n", #stmt, __FUNCTION__, __LINE__); \
        exit(-1); \
    } while(0)

#define ERROR(...) \
    if (__dbg_lvl__ >= DBG_LVL_ERROR) do { \
        fprintf(stderr, "<error> "__VA_ARGS__); \
        fprintf(stderr, " @%s #%d\n", __FUNCTION__, __LINE__); \
    } while(0)

#define WARNING(...) \
    if (__dbg_lvl__ >= DBG_LVL_WARNING) do { \
        fprintf(stderr, "<warning> "__VA_ARGS__); \
        fprintf(stderr, " @%s #%d\n", __FUNCTION__, __LINE__); \
    } while(0)

#define DEBUG(...) \
    if (__dbg_lvl__ >= DBG_LVL_DEBUG) do { \
        fprintf(stderr, "<debug> "__VA_ARGS__); \
        fprintf(stderr, " @%s #%d\n", __FUNCTION__, __LINE__); \
    } while(0)

#define VERBOSE(...) \
    if (__dbg_lvl__ >= DBG_LVL_VERBOSE) do { \
        fprintf(stderr, "<verbose> "__VA_ARGS__); \
        fprintf(stderr, " @%s #%d\n", __FUNCTION__, __LINE__); \
    } while(0)

extern void hexdump(unsigned char *p, size_t s, unsigned char *base);
#else
#define ASSERT(...)
#define ERROR(...)
#define WARNING(...)
#define DEBUG(...)
#define VERBOSE(...)
#define hexdump(...)
#endif

typedef std::map<std::string, std::string> http_options;
extern std::string http_time(time_t now);
extern time_t get_http_time(const char * str);

#endif /* __COMMON_HPP__ */
