//
// Created by gackt on 2/3/20.
//

#ifndef PLIB_PROTOCALCLIENTTCP_H
#define PLIB_PROTOCALCLIENTTCP_H

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

class ProtocalClientTCP {
public:
    ProtocalClientTCP(char *servInetAddr, int servPort);

    virtual ~ProtocalClientTCP();

    const std::string& handle(const std::string &message);

    // Server Info
    int servPort;
    struct hostent *server;
    char *servInetAddr;
    int serverlen;

    // Socket
    int connfd;
    struct sockaddr_in servaddr;
};

#endif //PLIB_PROTOCALCLIENTTCP_H
