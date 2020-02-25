//
// Created by gackt on 11/24/19.
//

#include <cstring>
#include "ProtocalClientUDP.h"

ProtocalClientUDP::ProtocalClientUDP(char *servInetAddr, int servPort) : servInetAddr(servInetAddr),
                                                                         servPort(servPort) {
    connfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (connfd < 0) {
        fprintf(stderr, "Error, no such host as %s\n", servInetAddr);
        exit(0);
    }

    server = gethostbyname(servInetAddr);
    if (server == NULL) {
        fprintf(stderr, "Error, no such host as %s\n", servInetAddr);
        exit(0);
    }

    bzero((char *) &servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(servPort);
    memcpy(&servaddr.sin_addr.s_addr, (char *) server->h_addr, server->h_length);

    //bcopy((char *) server->h_addr,(char *) &servaddr.sin_addr.s_addr, server->h_length);
    //inet_pton(AF_INET, servInetAddr, &servaddr.sin_addr);
    // hostent *hp = gethostbyname(servInetAddr);

    serverlen = sizeof(servaddr);
    printf("Connected to UDP server, %s : %d\n", this->servInetAddr, this->servPort);
}

ProtocalClientUDP::~ProtocalClientUDP() {
    close(connfd);
    printf("UDP client exit\n");
}

const std::string &ProtocalClientUDP::handle(const std::string &message) {
    int remain_retry = MAXRETRY;
    do {
        int ssize = sendto(connfd, message.data(), message.length(), 0, (struct sockaddr *) &servaddr,
                           sizeof(this->servaddr));
        if (ssize > 0) { break; }
        remain_retry -= 1;
    } while (remain_retry > 0);

    char recv_buffer[MAXLINE];
    int rsize = recvfrom(connfd, recv_buffer, MAXLINE, 0, (struct sockaddr *) NULL, NULL);
    if (rsize == -1)
        return "no response";
    printf("response %s \n", recv_buffer);
    return std::move(std::string(recv_buffer, rsize));
}

