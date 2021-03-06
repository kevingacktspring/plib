//
// Created by gackt on 11/24/19.
//

#ifndef PLIB_PROTOCALSERVERUDP_H
#define PLIB_PROTOCALSERVERUDP_H

#include <fcntl.h>
#include <zconf.h>
#include <cstdio>
#include <sys/epoll.h>
#include <sys/resource.h>
#include <netinet/in.h>

#define MAXEPOLLSIZE 10000
#define MAXLINE 10240
#define MAXSIZE 1024
#define DEFAULT_LISTEN_QUEUE 1024

class ProtocalServerUDP {
public:
    ProtocalServerUDP(int servPort, int listenq);

    virtual ~ProtocalServerUDP();

    void handle_accpet(int epollfd, int listenfd);

    void add_event(int epollfd, int fd, int state);

    void do_write(int epollfd, int fd, char *buf);

    void do_read(int epollfd, int fd, char *buf);

    void delete_event(int epollfd, int fd, int state);

    void modify_event(int epollfd, int fd, int state);

    int initService();

    int handle(int connfd);

    int setnonblocking(int sockfd);

protected:
    struct epoll_event ev;
    struct epoll_event events[MAXEPOLLSIZE];

    struct sockaddr_in servaddr;

    int servPort;
    int listenq = DEFAULT_LISTEN_QUEUE;

    int listenfd, epfd, nfds = 0;
};


#endif //PLIB_PROTOCALSERVERUDP_H
