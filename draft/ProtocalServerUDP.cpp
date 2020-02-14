//
// Created by gackt on 11/24/19.
//

#include <netinet/in.h>
#include <strings.h>
#include <cstring>
#include "ProtocalServerUDP.h"
#include  <arpa/inet.h>

int ProtocalServerUDP::initService() {
    int listenfd, connfd, kdpfd, nfds, n, nread, acceptCount = 0;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t socklen = sizeof(struct sockaddr_in);

    char buf[MAXLINE];

    /* 设置每个进程允许打开的最大文件数 */
    rt.rlim_max = rt.rlim_cur = MAXEPOLLSIZE;
    if (setrlimit(RLIMIT_NOFILE, &rt) == -1) {
        perror("setrlimit error");
        return -1;
    }


    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(servPort);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        perror("can't create socket file");
        return -1;
    }

    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (setnonblocking(listenfd) < 0) {
        perror("setnonblock error");
    }

    if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(struct sockaddr)) == -1) {
        perror("bind error");
        return -1;
    }
    if (listen(listenfd, listenq) == -1) {
        perror("listen error");
        return -1;
    }

    kdpfd = epoll_create(MAXEPOLLSIZE);
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = listenfd;
    if (epoll_ctl(kdpfd, EPOLL_CTL_ADD, listenfd, &ev) < 0) {
        fprintf(stderr, "epoll set insertion error: fd=%d\n", listenfd);
        return -1;
    }

    printf("epollserver startup,port %d, max connection is %d, backlog is %d\n", servPort, MAXEPOLLSIZE, listenq);

    while (true) {
        nfds = epoll_wait(kdpfd, events, MAXEPOLLSIZE, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            continue;
        }

        for (n = 0; n < nfds; ++n) {
            int i, fd;
            fd = events[i].data.fd;
            //根据描述符的类型和事件类型进行处理
            if ((fd == listenfd) && (events[i].events & EPOLLIN))
                handle_accpet(kdpfd, listenfd);
            else if (events[i].events & EPOLLIN)
                do_read(kdpfd, fd, buf);
            else if (events[i].events & EPOLLOUT)
                do_write(kdpfd, fd, buf);
        }
    }
    close(listenfd);
    return 0;
}

void ProtocalServerUDP::delete_event(int epollfd, int fd, int state) {
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
}

void ProtocalServerUDP::modify_event(int epollfd, int fd, int state) {
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}

void ProtocalServerUDP::do_write(int epollfd, int fd, char *buf) {
    int nwrite;
    nwrite = write(fd, buf, strlen(buf));
    if (nwrite == -1) {
        perror("write error:");
        close(fd);
        delete_event(epollfd, fd, EPOLLOUT);
    } else
        modify_event(epollfd, fd, EPOLLIN);
    memset(buf, 0, MAXSIZE);
}

void ProtocalServerUDP::do_read(int epollfd, int fd, char *buf) {
    int nread;
    nread = read(fd, buf, MAXSIZE);
    if (nread == -1) {
        perror("read error:");
        close(fd);
        delete_event(epollfd, fd, EPOLLIN);
    } else if (nread == 0) {
        fprintf(stderr, "client close.\n");
        close(fd);
        delete_event(epollfd, fd, EPOLLIN);
    } else {
        printf("read message is : %s", buf);
        //修改描述符对应的事件，由读改为写
        modify_event(epollfd, fd, EPOLLOUT);
    }
}

void ProtocalServerUDP::add_event(int epollfd, int fd, int state) {
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}

void ProtocalServerUDP::handle_accpet(int epollfd, int listenfd) {
    int clifd;
    struct sockaddr_in cliaddr;
    socklen_t cliaddrlen;
    clifd = accept(listenfd, (struct sockaddr *) &cliaddr, &cliaddrlen);
    if (clifd == -1)
        perror("accpet error:");
    else {
        printf("accept a new client: %s:%d\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
        //添加一个客户描述符和事件
        add_event(epollfd, clifd, EPOLLIN);
    }
}

int ProtocalServerUDP::setnonblocking(int sockfd) {
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK) == -1) {
        return -1;
    }
    return 0;
}

int ProtocalServerUDP::handle(int connfd) {
    int nread;
    char buf[MAXLINE];
    nread = read(connfd, buf, MAXLINE);//读取客户端socket流

    if (nread == 0) {
        printf("client close the connection\n");
        close(connfd);
        return -1;
    }
    if (nread < 0) {
        perror("read error");
        close(connfd);
        return -1;
    }
    write(connfd, buf, nread);
    return 0;
}

