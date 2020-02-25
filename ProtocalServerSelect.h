//
// Created by gackt on 2/3/20.
//

#ifndef PLIB_PROTOCALSERVERSELECT_H
#define PLIB_PROTOCALSERVERSELECT_H

#include <fcntl.h>
#include <zconf.h>
#include <cstdio>
#include <sys/select.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <deque>

#define MAXEPOLLSIZE 10000
#define MAXLINE 10240
#define DEFAULT_LISTEN_QUEUE 128
#define BUFSIZE 4096

class ProtocalServerSelect {
public:
    ProtocalServerSelect(int servPort);

    virtual ~ProtocalServerSelect();

    int setnonblocking(int sockfd);

    int initService();

    const std::deque<int>::const_iterator rmBadFd(const std::deque<int>::const_iterator);

protected:
    int servPort;

    int listenfd; /* fd tcp listen */
    int udpfd;  /* fd epoll wait resp */

    struct timeval tv{10, 0};

    fd_set rset; /* read set for select */
    int maxfdp1; /* max fd + 1 used in select inform */
    int nready; /* returns the number of ready descriptors */

    std::deque<int> readque;

    struct sockaddr_in serveraddr; /* server's addr */
    struct sockaddr_in clientaddr; /* client addr */

    int clientlen; /* byte sieze of client's address */
    int msgsize; /* message byte size */
    int writesize; /* write byte size */

    char recvbuffer[BUFSIZE]; /* message buf */

    struct hostent *hostp; /* client host info */

    char *hostaddrp;  /* dotted decimal host addr string */
    int optval; /* flag value for setsockopt */
};

#endif //PLIB_PROTOCALSERVERSELECT_H
