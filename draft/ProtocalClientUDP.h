//
// Created by gackt on 11/24/19.
//

#ifndef PLIB_PROTOCALCLIENTUDP_H
#define PLIB_PROTOCALCLIENTUDP_H

#include  <unistd.h>
#include  <sys/types.h>       /* basic system data types */
#include  <sys/socket.h>      /* basic socket definitions */
#include  <netinet/in.h>      /* sockaddr_in{} and other Internet defns */
#include  <arpa/inet.h>       /* inet(3) functions */
#include  <netdb.h>           /* gethostbyname function */

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define MAXLINE 1024

void handle(int connfd);

void handle(int sockfd) {
    char sendline[MAXLINE];
    char recvline[MAXLINE];
    int n;
    for (;;) {
        if (fgets(sendline, MAXLINE, stdin) == NULL) {
            break;  // read eof
        }

        /*
        //也可以不用标准库的缓冲流,直接使用系统函数无缓存操作
        if (read(STDIN_FILENO, sendline, MAXLINE) == 0) {
            break;//read eof
        }
        */

        n = write(sockfd, sendline, strlen(sendline));
        n = read(sockfd, recvline, MAXLINE);
        if (n == 0) {
            printf("echoclient: server terminated prematurely\n");
            break;
        }
        write(STDOUT_FILENO, recvline, n);

        //如果用标准库的缓存流输出有时会出现问题
        //fputs(recvline, stdout);
    }
}

class ProtocalClientUDP {
public:
    void send();

};

#endif //PLIB_PROTOCALCLIENTUDP_H


