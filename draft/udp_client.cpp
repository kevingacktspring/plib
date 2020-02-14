//
// Created by gackt on 1/12/19.
//
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define DEST_PORT 8000
#define DSET_IP_ADDRESS  "127.0.0.1"

int send_by_udp(char *binptr, long required_size) {
    int sock_fd;

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("Socket Error...");
        exit(1);
    }

    struct sockaddr_in addr_serv;
    memset(&addr_serv, 0, sizeof(addr_serv));
    addr_serv.sin_family = AF_INET;
    addr_serv.sin_addr.s_addr = inet_addr(DSET_IP_ADDRESS);
    addr_serv.sin_port = htons(DEST_PORT);

    int len;
    len = sizeof(addr_serv);

    int send_num;
    int recv_num;
    char send_buf[256];
    char recv_buf[256];

    memset(send_buf, 0, 256);
    memcpy(send_buf, binptr, required_size);

    //try local test
    printf("Try local test in client");
    //feed_back *loadptr = load_bin_data(send_buf);
    //free_compressed(loadptr);

    printf("Client Send: \n");
    for (int i = 0; i < 256; ++i) {
        printf("%02X", send_buf+i);
    }

    send_num = sendto(sock_fd, send_buf, 256, 0, (struct sockaddr *) &addr_serv, len);

    if (send_num < 0) {
        perror("Sendto Error...");
        exit(1);
    }

    recv_num = recvfrom(sock_fd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *) &addr_serv, (socklen_t *) &len);

    if (recv_num < 0) {
        perror("Recvfrom Error...");
        exit(1);
    }

    recv_buf[recv_num] = '\0';
    printf("client receive %d bytes: %s\n", recv_num, recv_buf);

    close(sock_fd);

    return 0;
}
