//
// Created by gackt on 11/24/19.
//

#include "ProtocalClientUDP.h"

ProtocalClientUDP::ProtocalClientUDP(char *servInetAddr, int servPort) : servInetAddr(servInetAddr),
                                                                         servPort(servPort) {
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(servPort);
    inet_pton(AF_INET, servInetAddr, &servaddr.sin_addr);

    connfd = socket(AF_INET, SOCK_STREAM, 0);

    // hostent *hp = gethostbyname(servInetAddr);
    // memcpy(&servaddr.sin_addr, hp->h_addr_list[0], hp->h_length);

    // Not required
    if (connect(connfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        printf("Error connect to UDP server, %s : %d\n", this->servInetAddr, this->servPort);
    }

    printf("Connected to UDP server, %s : %d\n", this->servInetAddr, this->servPort);
}

ProtocalClientUDP::~ProtocalClientUDP() {
    close(connfd);
    printf("UDP client exit\n");
}

const std::string &ProtocalClientUDP::handle(int sockfd, const std::string &message) {
    int remain_retry = MAXRETRY;
    do {
        int ssize = sendto(sockfd, message.data(), message.length(), 0 | MSG_DONTWAIT, (struct sockaddr *) &servaddr,
                           sizeof(this->servaddr));
        if (ssize > 0) { break; }
        remain_retry -= 1;
    } while (remain_retry > 0);

    char recv_buffer[MAXLINE];
    int rsize = recvfrom(sockfd, recv_buffer, MAXLINE, 0, (struct sockaddr *) NULL, NULL);
    return std::move(std::string(recv_buffer, rsize));
}

