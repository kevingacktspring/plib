//
// Created by gackt on 11/24/19.
//

#include <cstring>
#include <fcntl.h>
#include "ProtocalClientTCP.h"

ProtocalClientTCP::ProtocalClientTCP(char *servInetAddr, int servPort) : servInetAddr(servInetAddr),
                                                                         servPort(servPort) {
    connfd = socket(AF_INET, SOCK_STREAM, 0);
    if (connfd < 0) {
        fprintf(stderr, "Error, no such host as %s\n", servInetAddr);
        exit(0);
    }

    server = gethostbyname(servInetAddr);
    if (server == NULL) {
        fprintf(stderr, "Error, no such host as %s\n", servInetAddr);
        exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(servPort);
    //inet_pton(AF_INET, servInetAddr, &servaddr.sin_addr);
    memcpy(&servaddr.sin_addr.s_addr, (char *) server->h_addr, server->h_length);

    serverlen = sizeof(servaddr);

    if (connect(connfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        printf("Error connect to server, %s : %d\n", this->servInetAddr, this->servPort);
        //exit(-1);
    }

    if (fcntl(connfd, F_SETFL, fcntl(connfd, F_GETFD, 0) | O_NONBLOCK) == -1) {
        perror("set nonblock fail");
        return;
    }

    printf("Connected to TCP server, %s : %d\n", this->servInetAddr, this->servPort);
}

ProtocalClientTCP::~ProtocalClientTCP() {
    close(connfd);
    printf("UDP client exit\n");
}

const std::string &ProtocalClientTCP::handle(const std::string &message) {
    int remain_retry = MAXRETRY;
    do {
        int ssize = write(connfd, message.data(), message.length());
        if (ssize > 0) { break; }
        remain_retry -= 1;
    } while (remain_retry > 0);

    char recv_buffer[MAXLINE];
    int rsize = read(connfd, recv_buffer, MAXLINE);
    if (rsize == -1)
        return "no response";
    printf("response %s \n", recv_buffer);
    return std::move(std::string(recv_buffer, rsize));
}
