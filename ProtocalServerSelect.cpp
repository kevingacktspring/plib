//
// Created by gackt on 2/3/20.
//

#include <strings.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include "ProtocalServerSelect.h"

int ProtocalServerSelect::initService() {
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
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (const void *) &optval, sizeof(int));
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
    if (setnonblocking(listenfd) < 0 || setnonblocking(udpfd)) {
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

    while (true) {
        /**
         * clear receive buffer
         */
        bzero(recvbuffer, BUFSIZE);

        // clear the descriptor set
        FD_ZERO(&rset);

        // set listenfd and udpfd in readset
        FD_SET(listenfd, &rset);
        FD_SET(udpfd, &rset);
        maxfdp1 = std::max(listenfd, udpfd) + 1;

        /**
         * readset will be clear when select returns
         */
        for (int tmp : readque) {
            FD_SET(tmp, &rset);
            maxfdp1 = std::max(maxfdp1, tmp) + 1;
        }

        /**
         * select the ready descriptor
         */
        nready = select(maxfdp1, &rset, NULL, NULL, &tv);

        if ((errno & EINVAL) || (errno & ENOMEM) || (errno & EBADF)) {
            perror("select() failed for EINVAL or ENOMEM or EBADF");
            return -1;
        }

        if (nready < 0) {
            perror("select() failed or time-out, continue");
            continue;
        }
        if (nready == 0) {
            continue;
        }

        /**
         * handle tcp accept
         */
        if (FD_ISSET(listenfd, &rset)) {
            int acceptedfd = accept(listenfd, (struct sockaddr *) &clientaddr, (socklen_t *) &clientlen);
            if (acceptedfd > 0) {
                setnonblocking(acceptedfd); /*set nonblock fd*/
                printf("new incoming connection - %d\n", acceptedfd);
                readque.push_back(acceptedfd);
            }
        }

        /**
         * handle udp read/write
         */
        if (FD_ISSET(udpfd, &rset)) {
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
        }

        /**
         * handle tcp read/write
         */
        for (auto it = readque.begin(); it != readque.end();) {
            if (FD_ISSET(*it, &rset)) {
                msgsize = read(*it, recvbuffer, BUFSIZE);
                if (msgsize == -1) {
                    printf("Error in read\n");
                    rmBadFd(it);
                    continue;
                }
                printf("Message from tcp client: %s \n", recvbuffer);
                std::string tmp("received vote: ");
                tmp.append(recvbuffer, msgsize);
                writesize = write(*it, tmp.data(), tmp.length());
                if (writesize < 0) {
                    printf("Error in sendto\n");
                    rmBadFd(it);
                    continue;
                }
                printf("server received %d/%d bytes: %s\n", strlen(recvbuffer), msgsize, recvbuffer);
            }
            ++it;
        }
        printf("pending select. \n");
    }
    return 0;
}


int ProtocalServerSelect::setnonblocking(int sockfd) {
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK) == -1) {
        return -1;
    }
    return 0;
}

ProtocalServerSelect::ProtocalServerSelect(int servPort) : servPort(servPort) {
    int ret = initService();
    if (ret < 0) {
        printf("start select server fail\n");
    }
}

ProtocalServerSelect::~ProtocalServerSelect() {
    close(udpfd);
    close(listenfd);
}

const std::deque<int>::const_iterator ProtocalServerSelect::rmBadFd(const std::deque<int>::const_iterator badfd) {
    int tmpfd = *badfd;
    const std::deque<int>::const_iterator itr = readque.erase(badfd);
    close(tmpfd);
    return itr;
}