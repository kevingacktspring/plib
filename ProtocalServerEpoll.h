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
#include <iostream>
#include <map>
#include <atomic>

#include "StateLocalData.h"
#include "ProtocalClientTCP.h"

#define MAXEPOLLSIZE 1000
#define DEFAULT_LISTEN_QUEUE 128
#define BUFSIZE 4096

class ProtocalServerEpoll {
public:
    ProtocalServerEpoll(VolatileState *node_state, VolatileState *(*cluster_state), uint8_t cluster_size);

    virtual ~ProtocalServerEpoll();

    int setnonblocking(int sockfd);

    int initService();

    void rmBadFd(const int badfd);

    void registerClient(ProtocalClientTCP *client);

    void addClientEpollEvent(ProtocalClientTCP *client);

    void modClientEpollEvent(ProtocalClientTCP *client);

    void closeEpollServer();

    bool regEpollResponse(const epoll_event comming_event, const int triggered_fd);

protected:
    /**
     * socket fds
     */
    int listenfd; /* fd tcp listen */
    int udpfd;  /* fd epoll wait resp */

    std::deque<int> readque;  // record all accepted fds

    struct sockaddr_in serveraddr; /* server's addr */
    struct sockaddr_in clientaddr; /* client addr */
    int clientlen; /* byte sieze of client's address */

    struct hostent *hostp; /* client host info */

    char *hostaddrp;  /* dotted decimal host addr string */
    int optval; /* flag value for setsockopt */

    /**
     * epoll fds
     */
    struct epoll_event tcp_listen_event;
    struct epoll_event udp_receive_event;

    struct epoll_event events[MAXEPOLLSIZE];

    int epfd; /*fd create epoll*/
    int nfds; /*fd epoll wait resp*/

    struct epoll_event tcp_accept_event;  /* new accepted socket*/

    /**
     * cluster
     */
    VolatileState *node_state;  // current-server state
    VolatileState **cluster_state;  // cluster-servers state
    uint8_t cluster_size;

    /**
     * read/write buffer
     */
    int msgsize; /* message byte size */
    int writesize; /* write byte size */
    char recvbuffer[BUFSIZE]; /* message buf */

    /**
     * map<fd, client>
     */
    std::map<int, ProtocalClientTCP *> clientmap;

    std::atomic_bool keep_running = ATOMIC_VAR_INIT(true);
};


#endif //PLIB_PROTOCALSERVEREPOLL_H
