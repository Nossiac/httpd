/**********************************************************************
 * Distributed under the terms of the GNU GPL, version 2
 * Author: Hua Shao <nossiac@163.com>
 **********************************************************************/

#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#include "common.hpp"
#include "server.hpp"


using namespace std;


void daemonize(int fd)
{
    pid_t pid = 0;

    pid = fork();
    if (pid < 0)
    {
        exit(1);
    }
    if (pid > 0)
        exit(0);

    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);

    umask(0);
    setsid();
}


void usage()
{
    printf("shttpd [options]\n");
    printf("    -h         display help message.\n");
    printf("    -D         daemon mode.\n");
    #ifdef __linux__
    printf("    -m <mode>  mode = epoll | select, default epoll.\n");
    #endif
    printf("    -l <file>  log file (only for daemon mode).\n");
}

int main(int argc, char ** argv)
{
    int opt = 0;
    bool daemon = false;
    #ifdef __linux__
    bool epoll = true;
    #endif
    const char * logfile = "/dev/null";
    Server * httpd;

    opt = getopt( argc, argv, "hDd:l:m:");
    while( opt != -1 ) {
        switch( opt ) {
            case 'D':
                daemon = true;
                break;

            case 'l':
                logfile = optarg;
                break;

#ifdef __linux__
            case 'm':
                if (optarg[0] != 'e')
                    epoll = false;
                break;
#endif
            default:
                usage();
                exit(0);
                break;
        }

        opt = getopt( argc, argv, "Dd:hl:m:?" );
    }

    int fd = open(logfile, O_WRONLY | O_APPEND | O_CREAT);
    if(fd < 0)
    {
        exit(1);
    }

    signal(SIGPIPE, SIG_IGN); // sendind data to a closed socket.

    if (daemon) daemonize(fd);

#ifdef __linux__
    if (epoll)
        httpd  = new ServerImplEpoll(8080);
    else
#endif
        httpd = new ServerImplSelect(8080);

    httpd->run();

    close(fd);
    return 0;
}


