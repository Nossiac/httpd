/**********************************************************************
 * Distributed under the terms of the GNU GPL, version 2
 * Author: Hua Shao <nossiac@163.com>
 **********************************************************************/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include "common.hpp"

using namespace std;


#ifdef __DEBUG__
int __dbg_lvl__ = DBG_LVL_ERROR;
#endif

string http_time(time_t now)
{
    char buf[1000];
    //time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
    return string(buf);
}

time_t get_http_time(const char * str)
{
    struct tm httptime;

    memset(&httptime, 0, sizeof(httptime));

    while(('\t' == *str || ' ' == *str)) str++;

    switch(str[3]) // ya, I know.
    {
        case ',':
            //[Sun, 06 Nov 1994 08:49:37 GMT] -- RFC822, updated by RFC1123
            strptime(str, "%a, %d %b %Y %H:%M:%S %Z", &httptime);
            break;
        case ' ':
            //[Sun Nov 6 08:49:37 1994] -- ANSI C's asctime format()
            strptime(str, "%a %b %d %H:%M:%S %Y", &httptime);
            break;
        default:
            //[Sunday, 06-Nov-94 08:49:37 GMT] -- RFC850, deprecated
            strptime(str, "%A, %d-%b-%y %H:%M:%S %Z", &httptime);
            break;
    }

    return mktime(&httptime);
}


#ifdef __DEBUG__
void hexdump(unsigned char *p, size_t s, unsigned char *base)
{
    int i, c;
    if ((size_t)s > (size_t)p)
    {
        s = (size_t)s - (size_t)p;
    }
    while ((int)s > 0)
    {
        if (base)
        {
            printf("%08zX: ", (size_t)p - (size_t)base);
        }
        else
        {
            printf("%08zX: ", (size_t)p);
        }
        for (i = 0;  i < 16;  i++)
        {
            if (i < (int)s)
            {
                printf("%02X ", p[i] & 0xFF);
            }
            else
            {
                printf("   ");
            }
            if (i == 7) printf(" ");
        }
        printf(" |");
        for (i = 0;  i < 16;  i++)
        {
            if (i < (int)s)
            {
                c = p[i] & 0xFF;
                if ((c < 0x20) || (c >= 0x7F)) c = '.';
            }
            else
            {
                c = ' ';
            }
            printf("%c", c);
        }
        printf("|\n");
        s -= 16;
        p += 16;
    }
}
#endif
