#include <iostream>
#include <signal.h>
#include "ProtocalServerEpoll.h"
#include "ProtocalServerSelect.h"

/* Catch Signal Handler function */
void signal_callback_handler(int signum) {
    printf("Caught signal SIGPIPE %d\n", signum);
}

int main() {
    std::cout << "App Server Entry here" << std::endl;

    /**
     * Catch Signal Handler SIGPIPE
     */
    signal(SIGPIPE, signal_callback_handler);

    /**
     * set max resource limit for procedure
     */
    struct rlimit rt;
    rt.rlim_max = rt.rlim_cur = MAXEPOLLSIZE;
    if (setrlimit(RLIMIT_NOFILE, &rt) == -1) {
        perror("setrlimit error");
        return -1;
    }

    ProtocalServerEpoll protocalServer(9090);
    // ProtocalServerSelect protocalServer(8080);
    protocalServer.initService();

    std::cout << "Success here" << std::endl;

    return 0;
}