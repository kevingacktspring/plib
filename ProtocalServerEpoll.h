//
// Created by gackt on 11/24/19.
//

#ifndef PLIB_PROTOCALSERVEREPOLL_H
#define PLIB_PROTOCALSERVEREPOLL_H

#include <fcntl.h>
#include <zconf.h>
#include <cstdio>
#include <sys/epoll.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <deque>

#define MAXEPOLLSIZE 10000
#define DEFAULT_LISTEN_QUEUE 128
#define BUFSIZE 4096

class ProtocalServerEpoll {
public:
    ProtocalServerEpoll(int servPort);

    virtual ~ProtocalServerEpoll();

    int setnonblocking(int sockfd);

    int initService();

    void rmBadFd(const int);


protected:
    struct epoll_event tcp_listen_event;
    struct epoll_event udp_receive_event;

    struct epoll_event events[MAXEPOLLSIZE];

    int servPort;

    int listenfd; /* fd tcp listen */
    int udpfd;  /* fd epoll wait resp */

    int epfd; /*fd create epoll*/
    int nfds; /*fd epoll wait resp*/

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

    struct epoll_event tcp_accept_event;  /* new accepted socket*/
};


#endif //PLIB_PROTOCALSERVEREPOLL_H
