//
// Created by gackt on 11/24/19.
//

#include <strings.h>
#include <cstring>
#include <cstdlib>
#include <errno.h>

#include "ProtocalServerEpoll.h"

int ProtocalServerEpoll::initService() {
    /**
     * tcp: create the parent socket
     */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        printf("Error opening tcp socket \n");
        exit(0);
    }

    /**
     * udp: create the udp socket
     */
    udpfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpfd < 0) {
        printf("Error opening udp socket \n");
        exit(0);
    }

    /**
     * setsocket: Handy debugging trick that lets
     * us return the server immediately after we kill it;
     * otherwise we have to wait about 20 secs
     * Eliminates "ERROR on binding: Address already in use" error.
     */
    optval = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, (const void *) &optval, sizeof(int));
    setsockopt(udpfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (const void *) &optval, sizeof(int));

    /**
     * set ipv4 content
     */
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(servPort);

    /**
     * set nonblock
     */
    if (setnonblocking(listenfd) < 0 || setnonblocking(udpfd) < 0) {
        perror("setnonblock error");
    }

    /**
     * tcp
     * bind: associate the parent socket with a port
     */
    if (bind(listenfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
        perror("bind error");
        return -1;
    }
    if (listen(listenfd, DEFAULT_LISTEN_QUEUE) == -1) {
        perror("listen error");
        return -1;
    }

    /**
     * udp
     * bind: associate the parent socket with a port
     */
    if (bind(udpfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
        perror("bind error");
        return -1;
    }

    /**
     *
     */
    clientlen = sizeof(clientaddr);

    epfd = epoll_create(MAXEPOLLSIZE);

    /**
     * epoll tcp listen event
     */
    tcp_listen_event.events = EPOLLIN | EPOLLET;
    tcp_listen_event.data.fd = listenfd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &tcp_listen_event) < 0) {
        fprintf(stderr, "epoll set insertion error: fd = %d \n", listenfd);
        return -1;
    }

    /**
   * epoll udp listen event
   */
    udp_receive_event.events = EPOLLIN | EPOLLET;
    udp_receive_event.data.fd = udpfd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, udpfd, &udp_receive_event) < 0) {
        fprintf(stderr, "epoll set insertion error: fd = %d \n", udpfd);
        return -1;
    }

    printf("epoll server startup, port %d, max connection is %d, backlog is %d \n", servPort, MAXEPOLLSIZE,
           DEFAULT_LISTEN_QUEUE);

    /**
     * clear receive buffer
     */
    bzero(recvbuffer, BUFSIZE);

    /**
     * event loop
     */
    while (true) {
        nfds = epoll_wait(epfd, events, MAXEPOLLSIZE, -1);

        /**
         * clear receive buffer
         */
        bzero(recvbuffer, BUFSIZE);

        if (nfds == -1) {
            perror("epoll wait");
            continue;
        }

        for (int i = 0; i < nfds; ++i) {
            epoll_event comming_event = events[i];
            int triggered_fd = comming_event.data.fd;

            /**
             * An error has occured on this fd, or the socket is not
             * ready for reading
             */
            if ((comming_event.events & EPOLLERR) || (comming_event.events & EPOLLHUP) ||
                (!(comming_event.events & EPOLLIN))) {
                fprintf(stderr, "epoll error\n");
                rmBadFd(triggered_fd);
                continue;
            }

            /**
             * deal tcp listen
             */
            if ((triggered_fd == listenfd) && (comming_event.events & EPOLLIN)) {
                int acceptedfd;
                while((acceptedfd = accept(listenfd, (struct sockaddr *) &clientaddr, (socklen_t *) &clientlen)) > 0) {
                    setnonblocking(acceptedfd); /*set nonblock*/
                    memset(&tcp_accept_event, 0, sizeof( struct epoll_event));
                    tcp_accept_event.events = EPOLLIN | EPOLLET;
                    tcp_accept_event.data.fd = acceptedfd;
                    if (epoll_ctl(epfd, EPOLL_CTL_ADD, acceptedfd, &tcp_accept_event) < 0) {
                        fprintf(stderr, "epoll set insertion error: fd = %d \n", acceptedfd);
                        exit(EXIT_FAILURE);
                    }
                    printf("new incoming connection - %d\n", acceptedfd);
                }
                if (acceptedfd == -1) {
                    if (errno != EAGAIN && errno != ECONNABORTED && errno != EPROTO && errno != EINTR) {
                        perror("accept");
                    }
                }
                continue;
            }

            /**
            * udp receive
            */
            if ((triggered_fd == udpfd) && (comming_event.events & EPOLLIN)) {
                msgsize = recvfrom(udpfd, recvbuffer, BUFSIZE, 0, (struct sockaddr *) &clientaddr,
                                   (socklen_t *) &clientlen);
                if (msgsize != -1) {
                    hostp = gethostbyaddr((const char *) &clientaddr.sin_addr.s_addr,
                                          sizeof(clientaddr.sin_addr.s_addr), AF_INET);
                    if (hostp != NULL)
                        printf("server received datagram from %s (%s)\n", hostp->h_name, hostaddrp);

                    hostaddrp = inet_ntoa(clientaddr.sin_addr);
                    if (hostaddrp == NULL)
                        printf("error on inet_ntoa\n");

                    writesize = sendto(udpfd, "got", 3, 0, (struct sockaddr *) &clientaddr, clientlen);
                    if (writesize < 0)
                        printf("error in sendto");

                    printf("server received %d/%d bytes: %s\n", strlen(recvbuffer), msgsize, recvbuffer);
                }
                continue;
            }

            /**
             * epoll read
             */
            if (comming_event.events & EPOLLIN) {
                int nread = 0;
                msgsize = 0;
                while ((nread = read(triggered_fd, recvbuffer+msgsize, BUFSIZE - 1)) > 0) {
                    msgsize += nread;
                }
                if (nread == -1 && errno != EAGAIN) {
                    perror("read error");
                    rmBadFd(triggered_fd);
                    continue;
                }
                printf("Message from tcp client: %s \n", recvbuffer);

                memset(&tcp_accept_event, 0, sizeof( struct epoll_event));
                tcp_accept_event.events = comming_event.events | EPOLLOUT;
                tcp_accept_event.data.fd = triggered_fd;
                if (epoll_ctl(epfd, EPOLL_CTL_MOD, triggered_fd, &tcp_accept_event) == -1) {
                    perror("epoll_ctl: mod error");
                    rmBadFd(triggered_fd);
                    continue;
                }
                /*
                msgsize = read(triggered_fd, recvbuffer, BUFSIZE);
                if (msgsize == -1) {
                    printf("Error in tcp read \n");
                    continue;
                }
                printf("Message from tcp client: %s \n", recvbuffer);
                char *tmp = "received vote\0";
                writesize = write(triggered_fd, tmp, strlen(tmp));
                if (writesize < 0) {
                    printf("Error in tcp write\n");
                    rmBadFd(triggered_fd);
                    continue;
                }
                */
                printf("Server received %d/%d bytes: %s\n", strlen(recvbuffer), msgsize, recvbuffer);
            }

            if (comming_event.events & EPOLLOUT) {
                sprintf(recvbuffer, "HTTP/1.1 200 OK\r\nContent-length: %d\r\n\r\nreceived vote.\0",15);
                int nwrite, datasize = strlen(recvbuffer);
                writesize = datasize;
                while (writesize > 0) {
                    nwrite = write(triggered_fd, recvbuffer + datasize - writesize, writesize);
                    if (nwrite < writesize) {
                        if (nwrite == -1 && errno != EAGAIN) {
                            perror("write error");
                            rmBadFd(triggered_fd);
                        }
                        break;
                    }
                    writesize -= nwrite;
                }
                memset(&tcp_accept_event, 0, sizeof( struct epoll_event));
                tcp_accept_event.events = comming_event.events & ~EPOLLOUT;
                tcp_accept_event.data.fd = triggered_fd;
                if (epoll_ctl(epfd, EPOLL_CTL_MOD, triggered_fd, &tcp_accept_event) == -1) {
                    perror("epoll_ctl: mod error");
                    rmBadFd(triggered_fd);
                    continue;
                }
                // close(triggered_fd);
            }
            printf("event-size: %d\n", sizeof(events)/sizeof(struct epoll_event));
        }
    }
    return 0;
}


int ProtocalServerEpoll::setnonblocking(int sockfd) {
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK) == -1) {
        return -1;
    }
    return 0;
}

ProtocalServerEpoll::ProtocalServerEpoll(int servPort) : servPort(servPort) {
    int ret = initService();
    if (ret < 0) {
        printf("start epoll server fail\n");
    }
}

ProtocalServerEpoll::~ProtocalServerEpoll() {
    close(udpfd);
    close(listenfd);
}

void ProtocalServerEpoll::rmBadFd(const int badfd) {
    if (epoll_ctl(epfd, EPOLL_CTL_DEL, badfd, NULL) < 0) {
        fprintf(stderr, "epoll delete fd error: fd = %d \n", badfd);
    }
    close(badfd);
}
