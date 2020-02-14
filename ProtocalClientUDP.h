//
// Created by gackt on 11/24/19.
//

#ifndef PLIB_PROTOCALCLIENTUDP_H
#define PLIB_PROTOCALCLIENTUDP_H

#include <unistd.h>
#include <sys/types.h>       /* basic system data types */
#include <sys/socket.h>      /* basic socket definitions */
#include <netinet/in.h>      /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>       /* inet(3) functions */
#include <netdb.h>           /* gethostbyname function */

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string>
#include <strings.h>

#define MAXLINE 4096
#define MAXRETRY 3

class ProtocalClientUDP {
public:
    ProtocalClientUDP(char *servInetAddr, int servPort);

    virtual ~ProtocalClientUDP();

    void send();

    const std::string& handle(int sockfd, const std::string &message);

    // Server Info
    char *servInetAddr;
    int servPort;

    // Socket
    int connfd;
    struct sockaddr_in servaddr;
};

#endif //PLIB_PROTOCALCLIENTUDP_H


