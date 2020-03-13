//
// Created by gackt on 3/13/20.
//

#ifndef PLIB_FDUTILS_H
#define PLIB_FDUTILS_H

#include <cstdio>
#include <fcntl.h>
#include <signal.h>
#include <string>
#include <string.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/socket.h>

/* Catch Signal Handler function */
static inline void signal_callback_handler(int signum) {
    printf("Caught signal SIGPIPE %d\n", signum);
}

/**
 * set max resource limit for procedure
 */
static inline void set_resource_limit(uint64_t limit) {
    struct rlimit rt;
    rt.rlim_max = rt.rlim_cur = limit;
    if (setrlimit(RLIMIT_NOFILE, &rt) == -1) {
        perror("setrlimit error");
        exit(-1);
    }
}

/**
 * set nonblock
 * @param fd
 * @return
 */
static inline int set_fd_nonblock(int fd) {
    if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK) == -1) {
        perror("set nonblock fail");
        return -1;
    }
    return 0;
}

/**
 * remove nonblock
 * @param fd
 * @return
 */
static inline int rm_fd_nonblock(int fd) {
    if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) & ~O_NONBLOCK) == -1) {
        perror("remove nonblock fail");
        return -1;
    }
    return 0;
}

/**
 *
 * @param sockfd
 */
static void print_socket_err_reason(int sockfd) {
    int error = 0;
    socklen_t errlen = sizeof(error);
    if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (void *) &error, &errlen) == 0) {
        printf("error = %s\n", strerror(error));
    }
}

static uint64_t random_between(uint64_t lower, uint64_t upper) {
    return lower + std::rand() % (upper - lower + 1);
}

static uint64_t current_time_nano() {
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return uint64_t(now.tv_sec) * 1000 * 1000 * 1000 + uint64_t(now.tv_nsec);
}

static std::string current_time_utc() {
    time_t now;
    thread_local static char buf[32] = {0};
    struct tm utc_tm;
    time(&now);
    gmtime_r(&now, &utc_tm);
    snprintf(buf, sizeof(buf), "%.4d-%.2d-%.2dT%.2d:%.2d:%.2d.000Z", utc_tm.tm_year + 1000,
             utc_tm.tm_mon + 1, utc_tm.tm_mday, utc_tm.tm_hour, utc_tm.tm_min, utc_tm.tm_sec);
    return std::string(buf);
}

/**
 * 1~2 second random
 * @return
 */
static uint64_t upd_election_time_nano() {
    uint64_t timeIntervalInNano = random_between(1000 * 1000 * 1000, 2000 * 1000 * 1000);
    return (current_time_nano() + timeIntervalInNano);
}

#endif //PLIB_FDUTILS_H
