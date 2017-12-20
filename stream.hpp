/**********************************************************************
 * Distributed under the terms of the GNU GPL, version 2
 * Author: Hua Shao <nossiac@163.com>
 **********************************************************************/

#ifndef __STREAM_HPP__
#define __STREAM_HPP__

#include <vector>
#include <string>
#include <cstddef>
#include <cstdio>
#include <cstring>


class HttpStream
{
private:
    std::vector<unsigned char> data;
    std::vector<unsigned char>::const_iterator pos;

public:
    HttpStream(const unsigned char * const s, int len);

    /* read data */
    unsigned int readchar();
    unsigned int peekchar();
    std::string readword();
    std::string readline();
    void skipblank();
    void skip(int offset);
};

#endif /* __STREAM_HPP__ */
