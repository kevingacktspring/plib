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

#include "FdUtils.h"
#include "StateLocalData.h"

#define MAXLINE 4096
#define MAXRETRY 3

class ProtocalClientTCP {
public:
    ProtocalClientTCP(VolatileState *node_state);

    virtual ~ProtocalClientTCP();

    const std::string& handle(const std::string &message);

    int doConnect();

    int closeConnSocket();

    // Server Info
    struct hostent *server;
    struct sockaddr_in servaddr;
    struct addrinfo hints;

    // Socket
    int connfd;

    /**
     * cluster
     */
     const VolatileState *node_state;  // current-server state
};

#endif //PLIB_PROTOCALCLIENTTCP_H
