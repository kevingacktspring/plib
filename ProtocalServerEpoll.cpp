//
// Created by gackt on 11/24/19.
//

#include <strings.h>
#include <cstring>
#include <cstdlib>
#include <errno.h>

#include "ProtocalServerEpoll.h"
#include "FdUtils.h"

int ProtocalServerEpoll::initService() {
    printf("Epoll service doing initialize. \n");
    pthread_setname_np(pthread_self(), "ProtocalServerEpoll");

    printf("epoll server try startup, port %d, max connection is %d, backlog is %d \n",
           node_state->servPort, MAXEPOLLSIZE, DEFAULT_LISTEN_QUEUE);

    /**
     * clear receive buffer
     */
    bzero(recvbuffer, BUFSIZE);

    /**
     * event loop
     */
    while (keep_running.load()) {
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
            if ((comming_event.events & (EPOLLERR | EPOLLHUP))
                || (!(comming_event.events & (EPOLLIN | EPOLLOUT)))) {
                // debug
                if (comming_event.events & EPOLLERR) {
                    fprintf(stderr, "EPOLLERR error \n");
                } else if (comming_event.events & EPOLLHUP) {
                    fprintf(stderr, "EPOLLHUP error \n");
                }
                fprintf(stderr, "epoll error\n");
                fprintf(stderr, "epoll error fd: %d \n", triggered_fd);
                rmBadFd(triggered_fd);
                continue;
            }

            /**
             * deal tcp listen
             */
            if ((triggered_fd == listenfd) && (comming_event.events & EPOLLIN)) {
                printf("EPOLLIN triggered for accept.\n");
                int acceptedfd;
                while ((acceptedfd = accept(listenfd, (struct sockaddr *) &clientaddr, (socklen_t *) &clientlen)) > 0) {
                    set_fd_nonblock(acceptedfd); /*set nonblock*/
                    memset(&tcp_accept_event, 0, sizeof(struct epoll_event));
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
                        perror("tcp listen epoll event: accept error");
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
                while ((nread = read(triggered_fd, recvbuffer + msgsize, BUFSIZE - 1)) > 0) {
                    msgsize += nread;
                }
                if (nread == -1 && errno != EAGAIN) {
                    perror("read error");
                    rmBadFd(triggered_fd);
                    continue;
                }

                if (msgsize == 0) {
                    printf("Here %s", recvbuffer);
                    printf("receive tcp close/fin zero");
                    rmBadFd(triggered_fd);
                    continue;
                }

                printf("Message from tcp client: %s \n", recvbuffer);

                //debug
                auto debug = std::string(recvbuffer).find("HTTP/1.1 200 OK");
                if (debug == std::string::npos) { // not the fake response
                    memset(&tcp_accept_event, 0, sizeof(struct epoll_event));
                    tcp_accept_event.events = comming_event.events | EPOLLOUT;
                    tcp_accept_event.data.fd = triggered_fd;
                    if (epoll_ctl(epfd, EPOLL_CTL_MOD, triggered_fd, &tcp_accept_event) == -1) {
                        perror("epoll_ctl: mod error");
                        rmBadFd(triggered_fd);
                        continue;
                    }
                }

                printf("Server received %d/%d bytes: %s\n", strlen(recvbuffer), msgsize, recvbuffer);
            }

            /**
             * epoll write
             */
            if (comming_event.events & EPOLLOUT) {
                fprintf(stdout, "Triggered EPOLLOUT, fd = %d \n", triggered_fd);
                sprintf(recvbuffer, "HTTP/1.1 200 OK\r\nContent-length: %d\r\n\r\nreceived vote\0\n", 15);
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
                memset(&tcp_accept_event, 0, sizeof(struct epoll_event));
                tcp_accept_event.events = (comming_event.events & ~EPOLLOUT) | EPOLLIN;
                tcp_accept_event.data.fd = triggered_fd;
                if (epoll_ctl(epfd, EPOLL_CTL_MOD, triggered_fd, &tcp_accept_event) == -1) {
                    perror("epoll_ctl: mod error");
                    rmBadFd(triggered_fd);
                    continue;
                }
            }
            printf("Done triggered-fd: %d \n", triggered_fd);
        }
    }
    return 0;
}

/**
 *
 * @param node_state
 * @param cluster_state
 * @param cluster_size
 */
ProtocalServerEpoll::ProtocalServerEpoll(VolatileState *node_state,
                                         VolatileState **cluster_state,
                                         uint8_t cluster_size) : node_state(node_state),
                                                                 cluster_state(cluster_state),
                                                                 cluster_size(cluster_size) {
    std::cout << " Server constructed: "
              << " node: " << node_state->nodeid
              << " address: " << node_state->servInetAddr
              << " port: " << node_state->servPort << std::endl;

    std::cout << " Cluster nodes: " << std::endl;
    for (int j = 0; j < cluster_size; ++j) {
        std::cout << " node: " << (*(cluster_state + j))->nodeid
                  << " address: " << (*(cluster_state + j))->servInetAddr
                  << " port: " << (*(cluster_state + j))->servPort << std::endl;
    }

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
    serveraddr.sin_port = htons(node_state->servPort);

    /**
     * set nonblock
     */
    if (set_fd_nonblock(listenfd) < 0 || set_fd_nonblock(udpfd) < 0) {
        perror("setnonblock error");
    }

    /**
     * tcp
     * bind: associate the parent socket with a port
     */
    if (bind(listenfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
        perror("bind error");
        exit(-1);
    }
    if (listen(listenfd, DEFAULT_LISTEN_QUEUE) == -1) {
        perror("listen error");
        exit(-1);
    }

    /**
     * udp
     * bind: associate the parent socket with a port
     */
    if (bind(udpfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
        perror("bind error");
        exit(-1);
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
        exit(-1);
    }

    /**
   * epoll udp listen event
   */
    udp_receive_event.events = EPOLLIN | EPOLLET;
    udp_receive_event.data.fd = udpfd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, udpfd, &udp_receive_event) < 0) {
        fprintf(stderr, "epoll set insertion error: fd = %d \n", udpfd);
        exit(-1);
    }

    printf("epoll server constructed complete. \n");
}

ProtocalServerEpoll::~ProtocalServerEpoll() {
    close(udpfd);
    close(listenfd);
}

void ProtocalServerEpoll::rmBadFd(const int badfd) {
    fprintf(stdout, "clean bad fd: fd = %d \n", badfd);

    /**
     * deal with listenfd / udpfd
     */
    if (badfd == listenfd) {
        fprintf(stdout, "ignore bad listen-fd: fd = %d is listen fd \n", badfd);
        epoll_ctl(epfd, EPOLL_CTL_MOD, listenfd, &tcp_listen_event);
        return;
    } else if (badfd == udpfd) {
        fprintf(stdout, "ignore bad udp-fd: fd = %d is udp fd \n", badfd);
        epoll_ctl(epfd, EPOLL_CTL_MOD, udpfd, &udp_receive_event);
        return;
    }

    /**
     * deal with tcp client fd
     */
    auto badclient = clientmap.find(badfd);
    if (badclient != clientmap.end()) {
        fprintf(stdout, "ignore bad-fd: fd = %d is client-fd node-id: %d  \n", badfd,
                badclient->second->node_state->nodeid);
        this->modClientEpollEvent(badclient->second);
        return;
    }

    /**
     * default read fd
     */
    if (epoll_ctl(epfd, EPOLL_CTL_DEL, badfd, NULL) < 0) {
        fprintf(stderr, "epoll delete fd error: fd = %d \n", badfd);
    }
    close(badfd);
}

/**
 * register client
 */
void ProtocalServerEpoll::registerClient(ProtocalClientTCP *client) {
    fprintf(stdout, "try register client node-id; %d \n", client->node_state->nodeid);

    /**
     * record to client-map
     */
    clientmap.insert({client->connfd, client});

    /**
     * try connect
     */
    if (client->doConnect() < 0) {
        fprintf(stdout, "fail register client node-id: %d \n", client->node_state->nodeid);
        return;
    }

    /**
     * epoll tcp listen event
     */
    this->addClientEpollEvent(client);
}

/**
 * register epoll event for tcp-client
 * @param client
 */
void ProtocalServerEpoll::addClientEpollEvent(ProtocalClientTCP *client) {
    struct epoll_event client_event;
    client_event.events = EPOLLOUT | EPOLLET;
    client_event.data.fd = client->connfd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, client->connfd, &client_event) < 0) {
        fprintf(stderr, "epoll set insertion error : fd = %d \n", client->connfd);
        if (errno == EBADF) {
            fprintf(stderr, "epoll ctl return EBADFD \n");
        }
    }
}

/**
 * register epoll event for tcp-client
 * @param client
 */
void ProtocalServerEpoll::modClientEpollEvent(ProtocalClientTCP *client) {
    struct epoll_event client_event;
    client_event.events = EPOLLOUT | EPOLLET;
    client_event.data.fd = client->connfd;
    if (epoll_ctl(epfd, EPOLL_CTL_MOD, client->connfd, &client_event) < 0) {
        fprintf(stderr, "epoll set insertion error : fd = %d \n", client->connfd);
        if (errno == EBADF) {
            fprintf(stderr, "epoll ctl return EBADFD \n");
        }
    }
}

void ProtocalServerEpoll::closeEpollServer() {
    this->keep_running.store(false);
}
