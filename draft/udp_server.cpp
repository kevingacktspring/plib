//
// Created by gackt on 1/12/19.
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "structure_fixed_size.h"
#include "compress_recursive.cpp"
#include "load_by_binary.cpp"
#include "udp_client.cpp"

#define SERV_PORT 8000

int main()
{
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_fd < 0)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in addr_serv;
    memset(&addr_serv, 0, sizeof(struct sockaddr_in));
    addr_serv.sin_family = AF_INET;
    addr_serv.sin_port = htons(SERV_PORT);
    addr_serv.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sock_fd, (struct sockaddr *)&addr_serv, sizeof(addr_serv)) < 0)
    {
        perror("Bind Error...");
        exit(1);
    }

    int len;
    len = sizeof(addr_serv);

    int recv_num;
    int send_num;
    char send_buf[256] = "Completed...";
    char recv_buf[256];

    memset(recv_buf, 0, 256);

    struct sockaddr_in addr_client;

    while(true)
    {
        printf("server wait:\n");

        recv_num = recvfrom(sock_fd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&addr_client, (socklen_t *)&len);

        printf("Received num: %d \n", recv_num);

        if(recv_num < 0)
        {
            perror("Recvfrom ERrror...");
            exit(1);
        }

        //recv_buf[recv_num] = '\0';
        printf("server receive %d bytes: %s \n", recv_num, recv_buf);

        feed_back *loadptr = load_bin_data(recv_buf);
        free_compressed(loadptr);

        send_num = sendto(sock_fd, send_buf, recv_num, 0, (struct sockaddr *)&addr_client, len);

        if(send_num < 0)
        {
            perror("sendto error:");
            exit(1);
        }
    }

    close(sock_fd);

    return 0;
}

