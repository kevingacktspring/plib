//
// Created by gackt on 11/24/19.
//

#include <cstring>
#include <fcntl.h>
#include "ProtocalClientTCP.h"

ProtocalClientTCP::ProtocalClientTCP(VolatileState *node_state) : node_state(node_state) {
    server = gethostbyname(node_state->servInetAddr);
    if (server == NULL) {
        fprintf(stderr, "Error, no such host as %s\n", node_state->servInetAddr);
        exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(node_state->servPort);
    memcpy(&servaddr.sin_addr.s_addr, (char *) server->h_addr, server->h_length);

    printf("Tcp client to node-id %d initialized, %s : %d \n", node_state->nodeid, node_state->servInetAddr,
           node_state->servPort);
}

ProtocalClientTCP::~ProtocalClientTCP() {
    close(connfd);
    printf("UDP client exit\n");
}

const std::string &ProtocalClientTCP::handle(const std::string &message) {
    //int ssize = send(connfd, message.data(), message.length(), MSG_DONTWAIT | MSG_NOSIGNAL);
    //int rsize = recv(connfd, recv_buffer, MAXLINE, MSG_DONTWAIT | MSG_NOSIGNAL);
    return "";
}

/**
 * connect to server
 */
int ProtocalClientTCP::doConnect() {
    // create socket
    connfd = socket(AF_INET, SOCK_STREAM, 0);
    // remove nonblack first
    rm_fd_nonblock(connfd);
    // do connect
    if (connect(connfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        printf("Error connect to node-id %d TCP server, %s : %d\n", node_state->nodeid, node_state->servInetAddr,
               node_state->servPort);
        return -1;
    }
    // set nonblock after connected
    set_fd_nonblock(connfd);
    printf("Connect fd: %d \n", connfd);
    printf("Connect to node-id %d TCP server, %s : %d \n", node_state->nodeid, node_state->servInetAddr,
           node_state->servPort);
    return 0;
}

int ProtocalClientTCP::closeConnSocket() {
    close(connfd);
}
