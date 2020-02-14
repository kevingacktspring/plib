#include <iostream>
#include "ProtocalServerUDP.h"
#include "ProtocalClientUDP.h"

int main() {
    std::cout << "Entry here" << std::endl;

    /* 设置每个进程允许打开的最大文件数 */
    struct rlimit rt;
    rt.rlim_max = rt.rlim_cur = MAXEPOLLSIZE;
    if (setrlimit(RLIMIT_NOFILE, &rt) == -1) {
        perror("setrlimit error");
        return -1;
    }
    ProtocalClientUDP clientUdp("127.0.0.1\0", 8080);
    //ProtocalServerUDP protocalServer(6888, 1024);
    //protocalServer.initService();

    std::cout << "Success here" << std::endl;

    return 0;
}