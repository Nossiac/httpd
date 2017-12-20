/**********************************************************************
 * Distributed under the terms of the GNU GPL, version 2
 * Author: Hua Shao <nossiac@163.com>
 **********************************************************************/

#include "stream.hpp"

using namespace std;

#define TB (0x09)      /* TAB */
#define LF (0x0A)      /* Line-Feed */
#define CR (0x0D)      /* Carriage-Return */
#define SP (0x20)      /* Space */


#define is_space(c) (CR == c || LF == c || TB == c || SP == c)

HttpStream::HttpStream(const unsigned char * const s,int len):data(s,s+len)
{
    pos = this->data.begin();
}


unsigned int HttpStream::readchar()
{
    if (this->pos != this->data.end())
        return *(this->pos++);
    else
        return EOF;
}


unsigned int HttpStream::peekchar()
{
    if (this->pos != this->data.end())
        return *(this->pos);
    else
        return EOF;
}


string HttpStream::readword()
{
    string str;
    char c;
    while(1)
    {
        c = this->readchar();
        if (EOF != c && !is_space((unsigned char)c))
            str.push_back(c);
        else
            break;
    }
    return str;
}


string HttpStream::readline()
{
    string str;
    char c;
    while(1)
    {
        c = this->readchar();
        if ('\r' == c && '\n' == this->peekchar())
        {
            this->skip(1);
            break;
        }
        str.push_back(c);
    }
    return str;
}


inline void HttpStream::skip(int offset)
{
    int len = offset;
    while(len-->=1) this->readchar();
}


void HttpStream::skipblank()
{
    char c;
    while(1)
    {
        c = this->peekchar();
        if (EOF == c || !is_space((unsigned char)c))
            break;
        else
            this->skip(1);
    }
}


